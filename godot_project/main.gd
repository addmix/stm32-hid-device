extends MarginContainer

var manager = GdSerialManager.new()

enum {
	RAW = 0,
	LINE_BUFFERED = 1,
	CUSTOM_DELIMITER = 2,
}

var connected_port : String

func _ready() -> void:
	manager.data_received.connect(_on_data)
	
	%SerialOutput.text = "No device connected\n"
	if manager.list_ports().has(0) and manager.open(manager.list_ports()[0].port_name, 115200, 1000, LINE_BUFFERED):
		connected_port = manager.list_ports()[0].port_name
		
		%SerialOutput.text = "Connected on port " + connected_port + "\n"


func _process(delta: float) -> void:
	manager.poll_events()

func _on_data(port: String, data: PackedByteArray):
	%SerialOutput.append_text(data.get_string_from_utf8().c_unescape())

func _on_button_pressed(text : String = "") -> void:
	var bytes_to_send : PackedByteArray = %CommandEdit.text.c_escape().to_utf8_buffer()
	%CommandEdit.text = ""
	manager.write(connected_port, bytes_to_send)
