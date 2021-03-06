#ifndef _NETCON_H_
#define _NETCON_H_
/* @(#$Id: netcon.h,v 1.11 2009-02-05 14:25:24 dockes Exp $  (C) 2002 Jean-Francois Dockes */
#include "refcntr.h"

/// A set of classes to manage client-server communication over a
/// connection-oriented network, or a pipe.
///
/// The listening/connection-accepting code currently only uses
/// TCP. The classes include client-side and server-side (accepting)
/// endpoints. Netcon also has server-side static code to handle a set
/// of client connections in parallel. This should be moved to a
/// friend class.
/// 
/// The client data transfer class can also be used for
/// timeout-protected/asynchronous io using a given fd (ie a pipe
/// descriptor)

/// Base class for all network endpoints:
class Netcon;
typedef RefCntr<Netcon> NetconP;

class Netcon {
public:
    enum Event {NETCONPOLL_READ = 0x1, NETCONPOLL_WRITE=0x2};
    Netcon() 
	: m_peer(0), m_fd(-1), m_ownfd(true), m_didtimo(0), m_wantedEvents(0)
    {}
    virtual ~Netcon();
    /// Remember whom we're talking to. We let external code do this because 
    /// the application may have a non-dns method to find the peer name.
    virtual void setpeer(const char *hostname);
    /// Retrieve the peer's hostname. Only works if it was set before !
    virtual const char *getpeer() {
	return m_peer ? (const char *)m_peer : "none";
    }
    /// Set or reset the TCP_NODELAY option. 
    virtual int settcpnodelay(int on = 1); 
    /// Did the last receive() call time out ? Resets the flag.
    virtual int timedout() {int s = m_didtimo; m_didtimo = 0; return s;}
    /// Return string version of last syscall error
    virtual char *sterror();
    /// Return the socket descriptor
    virtual int getfd() {return m_fd;}
    /// Close the current connection if it is open
    virtual void closeconn();
    /// Set/reset the non-blocking flag on the underlying fd. Returns
    /// prev state The default is that sockets are blocking except
    /// when added to the selectloop, or, transparently, to handle
    /// connection timeout issues.
    virtual int set_nonblock(int onoff);

    /// Decide what events the connection will be looking for
    /// (NETCONPOLL_READ, NETCONPOLL_WRITE)
    int setselevents(int evs) {return m_wantedEvents = evs;}
    /// Retrieve the connection's currently monitored set of events
    int getselevents() {return m_wantedEvents;}
    /// Add events to current set
    int addselevents(int evs) {return m_wantedEvents |= evs;}
    /// Clear events from current set
    int clearselevents(int evs) {return m_wantedEvents &= ~evs;}

    /// Utility function for a simplified select() interface: check one fd
    /// for reading or writing, for a specified maximum number of seconds.
    static int select1(int fd, int secs, int writing = 0);
    
    /// The selectloop interface is used to implement parallel servers. 
    /// All the interface is static (independant of any given object).

    /// Loop waiting for events on the connections and call the
    /// cando() method on the object when something happens (this will in 
    /// turn typically call the app callback set on the netcon). Possibly
    /// call the periodic handler (if set) at regular intervals.
    /// @return -1 for error. 0 if no descriptors left for i/o. 1 for periodic
    ///  timeout (should call back in after processing)
    static  int selectloop();
    /// Call from data handler: make selectloop return the param value
    static void selectloopReturn(int value) 
    {
	o_selectloopDoReturn = true;
	o_selectloopReturnValue = value;
    }
    /// Add a connection to be monitored (this will usually be called
    /// from the server's listen connection's accept callback)
    static  int addselcon(NetconP con, int);
    /// Remove a connection from the monitored set. Note that this is
    /// automatically called from the Netcon destructor, and when EOF is
    /// detected on a connection.
    static  int remselcon(NetconP con);

    /// Set a function to be called periodically, or a time before return.
    /// @param handler the function to be called. 
    ///  - if it is 0, selectloop() will return after ms mS (and can be called
    ///    again
    ///  - if it is not 0, it will be called at ms mS intervals. If its return 
    ///    value is <= 0, selectloop will return. 
    /// @param clp client data to be passed to handler at every call.
    /// @param ms milliseconds interval between handler calls or
    ///   before return. Set to 0 for no periodic handler.
    static  void setperiodichandler(int (*handler)(void *), void *clp, int ms);

protected:
    static bool o_selectloopDoReturn;
    static int  o_selectloopReturnValue;
    char *m_peer;	// Name of the connected host
    int   m_fd;
    bool  m_ownfd;
    int   m_didtimo;
    // Used when part of the selectloop map.
    short m_wantedEvents;
    // Method called by the selectloop when something can be done with a netcon
    virtual int cando(Netcon::Event reason) = 0;
};


///////////////////////
class NetconData;

/// Class for the application callback routine (when in
/// selectloop). It would be nicer to override cando() in a subclass
/// instead of setting a callback, but this can't be done conveniently
/// because accept() always creates a base NetconData (another way
/// would be to pass a factory function function to the listener, to create
/// NetconData derivatives).
class NetconWorker {
public:
    virtual ~NetconWorker() {}
    // NetconP holds a NetconData oeuf corse
    virtual int data(NetconData *con, Netcon::Event reason) = 0;
};

/// Base class for connections that actually transfer data. T
class NetconData : public Netcon {
public:
    NetconData() : m_buf(0), m_bufbase(0), m_bufbytes(0), m_bufsize(0)
    {}
    virtual ~NetconData();

    /// Write data to the connection.
    /// @param buf the data buffer
    /// @param cnt the number of bytes we should try to send
    /// @param expedited send data in as 'expedited' data.
    /// @return the count of bytes actually transferred, -1 if an
    ///  error occurred.
    virtual int send(const char *buf, int cnt, int expedited = 0);

    /// Read from the connection
    /// @param buf the data buffer
    /// @param cnt the number of bytes we should try to read (but we return 
    ///   as soon as we get data)
    /// @param timeo maximum number of seconds we should be waiting for data.
    /// @return the count of bytes actually read. 0 for timeout (call
    ///        didtimo() to discriminate from EOF). -1 if an error occurred.
    virtual int receive(char *buf, int cnt, int timeo = -1);
    /// Loop on receive until cnt bytes are actually read or a timeout occurs
    virtual int doreceive(char *buf, int cnt, int timeo = -1);
    /// Check for data being available for reading
    virtual int readready();
    /// Check for data being available for writing
    virtual int writeready();
    /// Read a line of text on an ascii connection
    virtual int getline(char *buf, int cnt, int timeo = -1);
    /// Set handler to be called when the connection is placed in the
    /// selectloop and an event occurs.
    virtual void setcallback(RefCntr<NetconWorker> user) {m_user = user;}

private:
    char *m_buf;	// Buffer. Only used when doing getline()s
    char *m_bufbase;    // Pointer to current 1st byte of useful data
    int m_bufbytes;	// Bytes of data.
    int m_bufsize;	// Total buffer size
    RefCntr<NetconWorker> m_user;
    virtual int cando(Netcon::Event reason);
};

/// Network endpoint, client side.
class NetconCli : public NetconData {	
public:
    NetconCli(int silent = 0) {m_silentconnectfailure = silent;}

    /// Open connection to specified host and named service. 
    int openconn(const char *host, char *serv, int timeo = -1);

    /// Open connection to specified host and numeric port. port is in
    /// HOST byte order
    int openconn(const char *host, unsigned int port, int timeo = -1);

    /// Reuse existing fd. 
    /// We DONT take ownership of the fd, and do no closin' EVEN on an
    /// explicit closeconn() or setconn() (use getfd(), close,
    /// setconn(-1) if you need to really close the fd and have no
    /// other copy).
    int setconn(int fd);

    /// Do not log message if openconn() fails.
    void setSilentFail(int onoff) {m_silentconnectfailure = onoff;}

private:
    int m_silentconnectfailure;	// No logging of connection failures if set
};

class NetconServCon;
#ifdef NETCON_ACCESSCONTROL
struct intarrayparam {
    int len;
    unsigned int *intarray;
};
#endif /* NETCON_ACCESSCONTROL */

/// Server listening end point.
///
/// if NETCON_ACCESSCONTROL is defined during compilation,
/// NetconServLis has primitive access control features: okaddrs holds
/// the host addresses for the hosts which we allow to connect to
/// us. okmasks holds the masks to be used for comparison.  okmasks
/// can be shorter than okaddrs, in which case we use the last entry
/// for all addrs beyond the masks array length. Both arrays are
/// retrieved from the configuration file when we create the endpoint
/// the key is either based on the service name (ex: cdpathdb_okaddrs,
/// cdpathdb_okmasks), or "default" if the service name is not found
/// (ex: default_okaddrs, default_okmasks)
class NetconServLis : public Netcon {
public:
    NetconServLis() {
#ifdef NETCON_ACCESSCONTROL
	permsinit = 0;
	okaddrs.len = okmasks.len = 0;
	okaddrs.intarray = okmasks.intarray = 0;
#endif /* NETCON_ACCESSCONTROL */
    }
    ~NetconServLis();
    /// Open named service.
    int openservice(char *serv, int backlog = 10);
    /// Open service by port number.
    int openservice(int port, int backlog = 10);
    /// Wait for incoming connection. Returned connected Netcon 
    NetconServCon *accept(int timeo = -1);

protected:
    /// This should be overriden in a derived class to handle incoming
    /// connections. It will usually call NetconServLis::accept(), and
    /// insert the new connection in the selectloop.
    virtual int cando(Netcon::Event reason);

private:
#ifdef NETCON_ACCESSCONTROL
    int permsinit;
    struct intarrayparam okaddrs;
    struct intarrayparam okmasks;
    int initperms(char *servicename);
    int initperms(int port);
    int checkperms(void *cli, int clilen);
#endif /* NETCON_ACCESSCONTROL */
};

/// Server-side accepted client connection. The only specific code
/// allows closing the listening endpoint in the child process (in the
/// case of a forking server)
class NetconServCon : public NetconData {	
public:
    NetconServCon(int newfd, Netcon* lis = 0) 
    { 
	m_liscon = lis;
	m_fd = newfd;
    }
    /// This is for forked servers that want to get rid of the main socket
    void closeLisCon() {
	if (m_liscon)
	    m_liscon->closeconn();
    }
private:
    Netcon* m_liscon;
};

#endif /* _NETCON_H_ */
