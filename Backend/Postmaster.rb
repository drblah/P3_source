require_relative 'TcpListener.rb'
require_relative 'UdpListener.rb'
require_relative 'DBconnector.rb'

require 'socket'
require 'json'
require 'yaml'


inPipe = Queue.new
uOutPipe = Queue.new
tOutPipe = Queue.new

if File.exists?('settings.yml')
	@settings = YAML.load_file('settings.yml')
else
	raise "Unable to load settings file. Make sure 'settings.yml' is in application directory."	
end

puts @settings

udp = UdpListener.new('0.0.0.0', @settings["hostPort"], inPipe, uOutPipe)
tcp = TcpListener.new(@settings["hostIP"], @settings["hostPort"], inPipe, tOutPipe)
@db = DBconnector.new(@settings["dbHost"], @settings["dbPort"], @settings["dbUser"], @settings["dbPassword"], @settings["dbName"])

def registrationReply(mac)
	device = @db.getDeviceByMac(mac)
	hostIP = @settings["hostIP"]
	regPacket = JSON.generate( {:type => "regok", :device_id => device["device_id"].to_i, :server_ip => hostIP, :server_port => 55056} )
	
	return regPacket

end

begin
	loop do
		input = inPipe.pop

		begin
			packet = JSON.parse(input)

		rescue JSON::ParserError => e
			puts "Could not parse incomming message."
			puts e
		end
		
		if defined? packet
			
			case packet["type"]

			when "register"
				puts "Registration is detected from device: #{packet["device_ip"]} - #{packet["device_MAC"]}"
				puts packet

				if @db.deviceExists?(packet)
					puts "Device is already in the database."

					uOutPipe.push( registrationReply(packet["device_MAC"]) )
				else
					puts "Saving device.."
					@db.registerDevice(packet)

					uOutPipe.push( registrationReply(packet["device_MAC"]) )
				end

			when "startreq"
				puts "Start request detected from device: #{packet["device_id"]}"
				puts packet
				msg = JSON.generate( {:type => "startok", :startTime => (Time.now + 5).to_i, :program => packet["program"].to_i } )
				puts "sending reply:\n\t #{msg}"
				tOutPipe.push msg
			
			when "sample"
				puts "Recieved data sample from device: #{packet["device_id"]}"
				puts packet

				# Check that device exists.
				if @db.deviceExists?(packet)
					puts "#{packet["device_id"]} is a know device. Saving data."
					
					@db.insertSample(packet)
					tOutPipe.push JSON.generate( {:type => "sampleOK"} )

				else
					puts "#{packet["device_id"]} is unregistered. Data NOT saved."
					tOutPipe.push nil
				end


			else
				puts "Recieved unknown packet type. type is: #{packet["type"]}"
				tOutPipe.push nil
			end
				
		end

	end
rescue Interrupt => e
	puts "\nShutting down."
	udp.finalize
	@db.close
	exit 0
end