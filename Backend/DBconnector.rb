require 'pg'

class DBconnector
	def initialize(host, port, userName, password, database)

		@server = PG::Connection.open(:host => host, :port => port, :dbname => database, :user => userName, :password => password)
		
		@server.prepare('countDevice', "SELECT COUNT(*) FROM devices WHERE device_addr = $1;")
		@server.prepare('countDeviceID', "SELECT COUNT(*) FROM devices WHERE device_id = $1;")
		@server.prepare('registerDevice', "INSERT INTO devices(device_profile, device_addr) VALUES ($1, $2);")
		@server.prepare('getDeviceByMac', "SELECT * FROM devices WHERE device_addr = $1;")
		@server.prepare('insertSample', "INSERT INTO device_data(dev_id, dev_operation, runtimestamp, measured) VALUES ($1, $2, to_timestamp($3), $4);")
	end
	
	def close
		@server.close
	end

	def deviceExists?(packet)
		
		count = 0

		if packet.has_key? "device_MAC"
			@server.exec_prepared('countDevice', [packet["device_MAC"]] ) do |result|
				result.each do |row|
					count = row["count"].to_i
				end
			end

		elsif packet.has_key? "device_id"
			@server.exec_prepared('countDeviceID', [packet["device_id"]] ) do |result|
				result.each do |row|
					count = row["count"].to_i
				end
			end						
		end

		if count == 1
			return true
		else
			return false
		end

	end

	# Gets a single device by MAC-address. Returns hash with all device attributes from database.
	def getDeviceByMac(mac)

		device = ""

		@server.exec_prepared('getDeviceByMac', [mac]) do |result|
			result.each do |row|
				device = row
			end
		end

		return device
	end

	def registerDevice(packet)

		begin
			@server.exec_prepared('registerDevice', [ packet["profile_id"], packet["device_MAC"] ])
		rescue PG::ForeignKeyViolation => e
			puts "Error: Cannot register device: #{packet["device_MAC"]}."
			puts " profile id: #{packet["profile_id"]} is not a valid profile registered in the database."
			raise
		end

	end

	def insertSample(packet)

		@server.exec_prepared('insertSample', [ packet["device_id"], packet["program"], packet["timeStamp"], packet["Data"] ])

		
	end
	
end