/**
 * D header file to interface with the
 * $(HTTP pubs.opengroup.org/onlinepubs/9699919799/basedefs/aio.h.html, Posix AIO API).
 *
 * Copyright: Copyright D Language Foundation 2018.
 * License:   $(LINK2 http://www.boost.org/LICENSE_1_0.txt, Boost License 1.0)
 * Authors:   $(HTTPS github.com/darredevil, Alexandru Razvan Caciulescu)
 */
module core.sys.posix.aio;

private import core.sys.posix.signal;
private import core.sys.posix.sys.types;

version (Posix):

extern (C):
@system:
@nogc:
nothrow:

version (CRuntime_Glibc)
{
    import core.sys.posix.config;

    struct aiocb
    {
        int aio_fildes;
        int aio_lio_opcode;
        int aio_reqprio;
        void* aio_buf;   //volatile
        size_t aio_nbytes;
        sigevent aio_sigevent;

        aiocb* __next_prio;
        int __abs_prio;
        int __policy;
        int __error_code;
        ssize_t __return_value;

        off_t aio_offset;
        ubyte[32] __glibc_reserved;
    }

    static if (__USE_LARGEFILE64)
    {
        struct aiocb64
        {
            int aio_fildes;
            int aio_lio_opcode;
            int aio_reqprio;
            void* aio_buf;   //volatile
            size_t aio_nbytes;
            sigevent aio_sigevent;

            aiocb* __next_prio;
            int __abs_prio;
            int __policy;
            int __error_code;
            ssize_t __return_value;

            off_t aio_offset;
            ubyte[32] __glibc_reserved;
        }
    }
}
else version (FreeBSD)
{
    struct __aiocb_private
    {
        long status;
        long error;
        void* kernelinfo;
    }

    struct aiocb
    {
        int aio_fildes;
        off_t aio_offset;
        void* aio_buf;   // volatile
        size_t aio_nbytes;
        private int[2] __spare;
        private void* _spare2__;
        int aio_lio_opcode;
        int aio_reqprio;
        private __aiocb_private _aiocb_private;
        sigevent aio_sigevent;
    }

    version = BSD_Posix;
}
else version (NetBSD)
{
    struct aiocb
    {
        off_t aio_offset;
        void* aio_buf;   // volatile
        size_t aio_nbytes;
        int aio_fildes;
        int aio_lio_opcode;
        int aio_reqprio;
        sigevent aio_sigevent;
        private int _state;
        private int _errno;
        private ssize_t _retval;
    }

    version = BSD_Posix;
}
else version (DragonFlyBSD)
{
    struct aiocb
    {
        int aio_fildes;
        off_t aio_offset;
        void* aio_buf;   // volatile
        size_t aio_nbytes;
        sigevent aio_sigevent;
        int aio_lio_opcode;
        int aio_reqprio;
        private int _aio_val;
        private int _aio_err;
    }

    version = BSD_Posix;
}
else version (Solaris)
{
    struct aio_result_t
    {
        ssize_t aio_return;
        int aio_errno;
    }

    struct aiocb
    {
        int aio_fildes;
        void* aio_buf;   // volatile
        size_t aio_nbytes;
        off_t aio_offset;
        int aio_reqprio;
        sigevent aio_sigevent;
        int aio_lio_opcode;
        aio_result_t aio_resultp;
        int aio_state;
        int[1] aio__pad;
    }
}
else
    static assert(false, "Unsupported platform");

/* Return values of cancelation function.  */
version (CRuntime_Glibc)
{
    enum
    {
        AIO_CANCELED,
        AIO_NOTCANCELED,
        AIO_ALLDONE
    }
}
else version (Solaris)
{
    enum
    {
        AIO_CANCELED,
        AIO_ALLDONE,
        AIO_NOTCANCELED
    }
}
else version (BSD_Posix)
{
    enum
    {
        AIO_CANCELED,
        AIO_NOTCANCELED,
        AIO_ALLDONE
    }
}

/* Operation codes for `aio_lio_opcode'.  */
version (CRuntime_Glibc)
{
    enum
    {
        LIO_READ,
        LIO_WRITE,
        LIO_NOP
    }
}
else version (Solaris)
{
    enum
    {
        LIO_NOP,
        LIO_READ,
        LIO_WRITE,
    }
}
else version (BSD_Posix)
{
    enum
    {
        LIO_NOP,
        LIO_WRITE,
        LIO_READ
    }
}

/* Synchronization options for `lio_listio' function.  */
version (CRuntime_Glibc)
{
    enum
    {
        LIO_WAIT,
        LIO_NOWAIT
    }
}
else version (Solaris)
{
    enum
    {
        LIO_NOWAIT,
        LIO_WAIT
    }
}
else version (BSD_Posix)
{
    enum
    {
        LIO_NOWAIT,
        LIO_WAIT
    }
}

/* Functions implementing POSIX AIO.  */
version (CRuntime_Glibc)
{
    static if (__USE_LARGEFILE64)
    {
        int aio_read64(aiocb64* aiocbp);
        int aio_write64(aiocb64* aiocbp);
        int aio_fsync64(int op, aiocb64* aiocbp);
        int aio_error64(const(aiocb64)* aiocbp);
        ssize_t aio_return64(aiocb64* aiocbp);
        int aio_suspend64(const(aiocb64*)* aiocb_list, int nitems, const(timespec)* timeout);
        int aio_cancel64(int fd, aiocb64* aiocbp);
        int lio_listio64(int mode, const(aiocb64*)* aiocb_list, int nitems, sigevent* sevp);

        alias aio_read = aio_read64;
        alias aio_write = aio_write64;
        alias aio_fsync = aio_fsync64;
        alias aio_error = aio_error64;
        alias aio_return = aio_return64;
        alias aio_suspend = aio_suspend64;
        alias aio_cancel = aio_cancel64;
        alias lio_listio = lio_listio64;
    }
    else
    {
        int aio_read(aiocb* aiocbp);
        int aio_write(aiocb* aiocbp);
        int aio_fsync(int op, aiocb* aiocbp);
        int aio_error(const(aiocb)* aiocbp);
        ssize_t aio_return(aiocb* aiocbp);
        int aio_suspend(const(aiocb*)* aiocb_list, int nitems, const(timespec)* timeout);
        int aio_cancel(int fd, aiocb* aiocbp);
        int lio_listio(int mode, const(aiocb*)* aiocb_list, int nitems, sigevent* sevp);
    }
}
else
{
    int aio_read(aiocb* aiocbp);
    int aio_write(aiocb* aiocbp);
    int aio_fsync(int op, aiocb* aiocbp);
    int aio_error(const(aiocb)* aiocbp);
    ssize_t aio_return(aiocb* aiocbp);
    int aio_suspend(const(aiocb*)* aiocb_list, int nitems, const(timespec)* timeout);
    int aio_cancel(int fd, aiocb* aiocbp);
    int lio_listio(int mode, const(aiocb*)* aiocb_list, int nitems, sigevent* sevp);
}

/* Functions outside/extending POSIX requirement.  */
version (CRuntime_Glibc)
{
    static if (__USE_GNU)
    {
        /* To customize the implementation one can use the following struct.  */
        struct aioinit
        {
            int aio_threads;
            int aio_num;
            int aio_locks;
            int aio_usedba;
            int aio_debug;
            int aio_numusers;
            int aio_idle_time;
            int aio_reserved;
        }

        void aio_init(const(aioinit)* init);
    }
}
else version (FreeBSD)
{
    int aio_waitcomplete(aiocb** aiocb_list, const(timespec)* timeout);
    int aio_mlock(aiocb* aiocbp);
}
else version (DragonFlyBSD)
{
    int aio_waitcomplete(aiocb** aiocb_list, const(timespec)* timeout);
}
