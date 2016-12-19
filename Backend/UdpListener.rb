require 'celluloid/current'
require 'celluloid/io'

class UdpListener
  include Celluloid::IO
    finalizer :shutdown

    def initialize(host, port, inPipe, outPipe)
        puts "*** Starting echo server on #{host}:#{port}"

        # Since we included Celluloid::IO, we're actually making a
        # Celluloid::IO::TCPServer here
        @server = UDPSocket.new
        @server.bind(host, port)

        @inPipe = inPipe
        @outPipe = outPipe

        async.run
    end

    def shutdown
        @server.close if @server
    end

    def run
        loop { handle_connection }
    end

    def handle_connection
        data, (_, port, host) = @server.recvfrom(1024)
        puts "*** Received UDP connection from #{host}:#{port}"
        @inPipe.push(data)

        reply = @outPipe.pop

        @server.send(reply, 0, host, port)
    rescue => error
        puts "*** #{error.inspect}"
    end
end