require 'celluloid/current'
require 'celluloid/io'

class TcpListener
  include Celluloid::IO
  finalizer :shutdown

  def initialize(host, port, inPipe, outPipe)
    puts "*** Starting echo server on #{host}:#{port}"

    # Since we included Celluloid::IO, we're actually making a
    # Celluloid::IO::TCPServer here
    @server = TCPServer.new(host, port)
    @inPipe = inPipe
    @outPipe = outPipe

    async.run
  end

  def shutdown
    @server.close if @server
  end

  def run
    loop { async.handle_connection @server.accept }
  end

  def handle_connection(socket)
    _, port, host = socket.peeraddr
    puts "*** Received TCP connection from #{host}:#{port}"
    @inPipe.push socket.readpartial(255)

    reply = @outPipe.pop

    if not reply == nil
      socket.write reply  
    else
      socket.write "Nak"
    end

    socket.close
  rescue EOFError
    puts "*** #{host}:#{port} disconnected"
    socket.close
  end
end
