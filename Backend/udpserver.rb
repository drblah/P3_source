require 'socket'
require 'json'
addr = ['0.0.0.0', 55056]  # host, port

sendAddr = ['10.0.0.111', 33334]  # host, port
# Create socket and bind to address
UDPSock = UDPSocket.new
UDPSock.bind(addr[0], addr[1])

sendSock = UDPSocket.new
sendSock.bind(sendAddr[0], sendAddr[1])

workers = []

workers << Thread.new {
	while true
		data, addr = UDPSock.recvfrom(1024) # if this number is too low it will drop the larger packets and never give them to you

		packet = JSON.parse(data)

		if packet["type"] == "register"

			puts "Recieved register request from #{packet["device_ip"]}"
			puts "Device is of type #{packet["profile_id"]} and MAC is #{packet["device_MAC"]}"
			
			msg = JSON.generate( {:type => "regok", :device_id => 123, :server_ip => "10.0.0.111", :server_port => 55056} )
			#msg = JSON.generate( {:device_id => 123, :server_ip => "10.0.0.100", :server_port => 1337} )

			sendSock.send(msg, 0, "#{packet["device_ip"]}", 55056)		
		end

		puts "From addr: '%s', msg: '%s'" % [addr[0], data]
	end	
}

workers << Thread.new {
	while true
		server = TCPServer.new 55056

		loop do

			client = server.accept
			line = client.gets

			packet = JSON.parse(line)

			if packet["type"] == "startreq"
				puts line

				msg = JSON.generate( {:type => "startok", :startTime => (Time.now + 15).to_i} )
				client.print msg
				

			elsif packet["type"] == "sample"
				puts line

				msg = JSON.generate( {:type => "sampleOK"} )
				client.print msg

			end

			client.close

		end
		
	end
}

workers.each{ |thr| thr.join }