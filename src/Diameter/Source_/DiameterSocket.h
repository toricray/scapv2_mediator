#ifndef __DIAMETERSOCKET_H__
#define __DIAMETERSOCKET_H__

#ifdef WIN32
#  include <Winsock2.h>
#else
#  include <errno.h>
#  include <fcntl.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/poll.h>
#  include <unistd.h>
#endif

//#ifndef SOL_TCP
//	#define SOL_TCP IPPROTO_TCP
//#endif


namespace CoreDiam {

//----------------------------------------------------------------------------
// DiamSocket
//----------------------------------------------------------------------------
class DiamSocket
{
public:
#if defined WIN32 || defined _WIN32 || defined _WINDOWS
	typedef SOCKET SocketHandle;

	enum {
		DS_SOCKET_ERROR   = SOCKET_ERROR,
		DS_INVALID_SOCKET = INVALID_SOCKET
	};
#else
	typedef int SocketHandle;

	enum {
		DS_SOCKET_ERROR   = -1,
		DS_INVALID_SOCKET = -1
	};
#endif

	//struct apr_pollfd_t {
	//	apr_pool_t     *p;              
	//	apr_datatype_e  desc_type;   
	//	apr_int16_t     reqevents;      
	//	apr_int16_t     rtnevents;      
	//	apr_descriptor  desc;        
	//	void           *client_data;          
	//};

	enum {
		// Requestable events. If poll(2) finds any of these set, they are
		// copied to revents on return.
		DS_POLLIN       = 0x0001,       // any readable data available
		DS_POLLPRI      = 0x0002,       // OOB/Urgent readable data
		DS_POLLOUT      = 0x0004,       // file descriptor is writeable
		DS_POLLRDNORM   = 0x0040,       // non-OOB/URG data available
		DS_POLLWRNORM   = DS_POLLOUT,   // no write type differentiation
		DS_POLLRDBAND   = 0x0080,       // OOB/Urgent readable data
		DS_POLLWRBAND   = 0x0100,       // OOB/Urgent data can be written

		// These events are set if they occur regardless of whether they were
		// requested.
		DS_POLLERR      = 0x0008,       // some poll error occurred
		DS_POLLHUP      = 0x0010,       // file descriptor was "hung up"
		DS_POLLNVAL     = 0x0020        // requested events "invalid"
	};

	struct PollEntry
	{
		DiamSocket::SocketHandle handle;     // descriptor
		short                    reqEvents;  // requested events
		short                    retEvents;  // returned events
	};

public:
#if defined WIN32 || defined _WIN32 || defined _WINDOWS
	static SocketHandle accept(SocketHandle listener, sockaddr_in *addr) {
		int len = sizeof(*addr);
		SocketHandle handle = ::accept(listener, (sockaddr *)addr, &len);
		return handle;
	}

	static int          close(SocketHandle handle) {
		return ::closesocket(handle);
	}

	static int          cork(SocketHandle handle) {
		return 0;
	}

	static int          getLastError() {
		return ::WSAGetLastError();
	}

	static bool         initialize() {
		WORD    wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		return (0 == ::WSAStartup(wVersionRequested, &wsaData));
	}

	static int          poll(PollEntry *entries, unsigned long count, int timeout) {
		// since windows vista:
		//int WSAAPI WSAPoll(
		//_Inout_  WSAPOLLFD fdarray[],
		//_In_     ULONG nfds,
		//_In_     INT timeout
		//);

		timeval tv = {timeout/1000, 1000*(timeout%1000)};

		fd_set rfds;
		fd_set wfds;
		int    nfds = 0;

		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//FD_SET(m_ListenHandle, &FdSet);

		PollEntry *entry = entries;
		for(unsigned long i = count; i > 0; --i, ++entry)
		{
			if(entry->reqEvents & DS_POLLIN)
			{
				FD_SET(entry->handle, &rfds);
				if(nfds < (int)entry->handle)
					nfds = (int)entry->handle;
			}

			if(entry->reqEvents & DS_POLLOUT)
			{
				FD_SET(entry->handle, &wfds);
				if(nfds < (int)entry->handle)
					nfds = (int)entry->handle;
			}
		}

		int result = ::select(nfds + 1, &rfds, &wfds, NULL, &tv);

		entry = entries;
		for(unsigned long i = count; i > 0; --i, ++entry)
		{
			if(FD_ISSET(entry->handle, &rfds))
				entry->retEvents |= DS_POLLIN;

			if(FD_ISSET(entry->handle, &wfds))
				entry->retEvents |= DS_POLLOUT;
		}

		return result;
	}

	static int          shutdown(SocketHandle handle) {
		return ::shutdown(handle, SD_BOTH);
	}

	static int          send(SocketHandle handle, const char *buf, int len) {
		int flags = 0;
		return ::send(handle, buf, len, flags);
	}

	static bool         setBlocking(SocketHandle handle, bool state) {
		u_long arg = state? 1: 0;
		return DiamSocket::DS_SOCKET_ERROR != ::ioctlsocket(handle, FIONBIO, &arg);
	}

	static int          uncork(SocketHandle handle) {
		return 0;
	}
#else  // Linux
	static SocketHandle accept(SocketHandle listener, sockaddr_in *addr) {
		socklen_t len = sizeof(*addr);
		SocketHandle handle = ::accept(listener, (sockaddr *)addr, &len);
		if(DiamSocket::DS_INVALID_SOCKET != handle)
		{
			int yes = 1;
			int result = ::setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));  //result = ::setsockopt(handle, SOL_TCP, TCP_NODELAY, &yes, sizeof(yes));
			if(result == DiamSocket::DS_SOCKET_ERROR)
			{
				//
			}
		}
		return handle;
	}

	static int          close(SocketHandle handle) {
		return ::close(handle);
	}

	static int          cork(SocketHandle handle) {
		int yes = 1;
		int result = ::setsockopt(handle, IPPROTO_TCP, TCP_CORK, &yes, sizeof(yes));  //int result = ::setsockopt(handle, SOL_TCP, TCP_CORK, &yes, sizeof(yes));
		if(result == DiamSocket::DS_SOCKET_ERROR)
		{
			//
		}
		return result;
	}

	static int          getLastError() {
		return errno;
	}

	static bool         initialize() {
		return true;
	}

	static int          poll(PollEntry *entries, unsigned long count, int timeout) {
		return ::poll((pollfd *)entries, count, timeout);
	}

	static int          send(SocketHandle handle, const char *buf, int len) {
		int flags = MSG_NOSIGNAL; // | MSG_MORE;
		return ::send(handle, buf, len, flags);
	}

	static bool         setBlocking(SocketHandle handle, bool state) {
		int flags = ::fcntl(handle, F_GETFL, 0);
		if(flags == DiamSocket::DS_SOCKET_ERROR)
			return false;
		int flagNonBlock = O_NONBLOCK;
		int compare = flags & flagNonBlock;
		if(state == (compare == 0))
			return true;
		if(state)
			flags ^= O_NONBLOCK;
		else
			flags |= O_NONBLOCK;
		if(DiamSocket::DS_SOCKET_ERROR == ::fcntl(handle, F_SETFL, flags))
			return false;
		return true;
	}

	static int          shutdown(SocketHandle handle) {
		return ::shutdown(handle, SHUT_RDWR);
	}

	static int          uncork(SocketHandle handle) {
		int no = 0;
		int result = ::setsockopt(handle, IPPROTO_TCP, TCP_CORK, &no, sizeof(no)); //int result = ::setsockopt(handle, SOL_TCP, TCP_CORK, &no, sizeof(no));
		if(result == DiamSocket::DS_SOCKET_ERROR)
		{
			//
		}
		return result;
	}
#endif
	static int          recv(SocketHandle handle, char *buf, int len) {
		return ::recv(handle, buf, len, 0);
	}

	static bool         setBufferSizes(SocketHandle handle, int rcvBufSize, int sndBufSize) {
		if(rcvBufSize)
		{
			if(DiamSocket::DS_SOCKET_ERROR == ::setsockopt(handle, SOL_SOCKET, SO_RCVBUF, (const char *)&rcvBufSize, sizeof(rcvBufSize)))
				return false;
		}
		
		if(sndBufSize)
		{
			if(DiamSocket::DS_SOCKET_ERROR == ::setsockopt(handle, SOL_SOCKET, SO_SNDBUF, (const char *)&sndBufSize, sizeof(sndBufSize)))
				return false;
		}

		return true;
	}

	static SocketHandle accept(SocketHandle listener, bool asBlocking, sockaddr_in *addr) {
		SocketHandle handle = DiamSocket::accept(listener, addr);
		if(DiamSocket::DS_INVALID_SOCKET != handle)
		{
			//*/
			if(!DiamSocket::setBlocking(handle, asBlocking))
			{
				DiamSocket::shutdown(handle);
				DiamSocket::close(handle);
				handle = DiamSocket::DS_INVALID_SOCKET;
			}
			//*/
		}
		return handle;
	}
};

}  // namespace CoreDiam

#endif  // __DIAMETERSOCKET_H__
