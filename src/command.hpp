/*
    Copyright (c) 2007-2016 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_COMMAND_HPP_INCLUDED__
#define __ZMQ_COMMAND_HPP_INCLUDED__

#include "stdint.hpp"

namespace zmq
{

    class object_t;
    class own_t;
    struct i_engine;
    class pipe_t;
    class socket_base_t;

    //  This structure defines the commands that can be sent between threads.

#ifdef _MSC_VER
    __declspec(align(64)) struct command_t
#else
    struct command_t
#endif
    {
        //  Object to process the command.
        zmq::object_t *destination;

        enum type_t
        {
            stop,
            plug,
            own,
            attach,
            bind,
			fd_assoc,
            activate_read,
            activate_write,
            hiccup,
            pipe_term,
            pipe_term_ack,
            term_req,
            term,
            term_ack,
            reap,
            reaped,
            inproc_connected,
            done
        } type;

        union args_t
        {

            //  Sent to I/O thread to let it know that it should
            //  terminate itself.
            struct {
            } stop;

            //  Sent to I/O object to make it register with its I/O thread.
            struct {
            } plug;

            //  Sent to socket to let it know about the newly created object.
            struct {
                zmq::own_t *object;
            } own;

            //  Attach the engine to the session. If engine is NULL, it informs
            //  session that the connection have failed.
            struct {
                struct i_engine *engine;
            } attach;

            //  Sent from session to socket to establish pipe(s) between them.
            //  Caller have used inc_seqnum beforehand sending the command.
            struct {
                zmq::pipe_t *pipe;
            } bind;

			struct {
				zmq::pipe_t *pipe;
				int fd;
			} fd_assoc;
			
            //  Sent by pipe writer to inform dormant pipe reader that there
            //  are messages in the pipe.
            struct {
            } activate_read;

            //  Sent by pipe reader to inform pipe writer about how many
            //  messages it has read so far.
            struct {
                uint64_t msgs_read;
            } activate_write;

            //  Sent by pipe reader to writer after creating a new inpipe.
            //  The parameter is actually of type pipe_t::upipe_t, however,
            //  its definition is private so we'll have to do with void*.
            struct {
                void *pipe;
            } hiccup;

            //  Sent by pipe reader to pipe writer to ask it to terminate
            //  its end of the pipe.
            struct {
            } pipe_term;

            //  Pipe writer acknowledges pipe_term command.
            struct {
            } pipe_term_ack;

            //  Sent by I/O object ot the socket to request the shutdown of
            //  the I/O object.
            struct {
                zmq::own_t *object;
            } term_req;

            //  Sent by socket to I/O object to start its shutdown.
            struct {
                int linger;
            } term;

            //  Sent by I/O object to the socket to acknowledge it has
            //  shut down.
            struct {
            } term_ack;

            //  Transfers the ownership of the closed socket
            //  to the reaper thread.
            struct {
                zmq::socket_base_t *socket;
            } reap;

            //  Closed socket notifies the reaper that it's already deallocated.
            struct {
            } reaped;

            //  Sent by reaper thread to the term thread when all the sockets
            //  are successfully deallocated.
            struct {
            } done;

        } args;
#ifdef _MSC_VER
    };
#else
    } __attribute__((aligned(64)));
#endif
}

#endif
