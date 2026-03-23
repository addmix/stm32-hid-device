extends MarginContainer

var serial = GdSerialManager.new()

var connected_port : String

func _ready() -> void:
	serial.data_received.connect(_on_data)
	serial.port_disconnected.connect(_on_disconnected)
	
	%SerialOutput.text = ""
	attempt_connection()

func _process(delta: float) -> void:
	serial.poll_events()

func attempt_connection() -> void:
	%SerialOutput.text += "Waiting to connect\n"
	
	#waits until connection is successful.
	while not (serial.list_ports().has(0) and serial.open_buffered(serial.list_ports()[0].port_name, 115200, 1000, GdSerialManager.MODE_LINE_BUFFERED)):
		await get_tree().create_timer(1.0).timeout
	
	connected_port = serial.list_ports()[0].port_name
	%SerialOutput.text += "Connected on port " + connected_port + "\n"

func _on_disconnected(port : String) -> void:
	connected_port = ""
	
	%SerialOutput.text += "Device disconnected from port " + connected_port + "\n"
	
	attempt_connection()

func _on_data(port: String, data: PackedByteArray) -> void:
	%SerialOutput.text += data.get_string_from_utf8().c_unescape()

func write(data : PackedByteArray) -> void:
	serial.write(connected_port, data)
