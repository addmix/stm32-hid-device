extends Button
class_name PinButton

@export var pin : PinDeclaration = PinDeclaration.new()
@export var input_mappings : Array[InputMapping] = []
#TODO: maybe make the button text show some info about the pin/mappings?

const pin_editor = preload("res://Configurator/pin_editor.tscn")
const input_editor = preload("res://Configurator/InputEditor.tscn")

#todo add right-click logic
# - right click menu should provide option to edit pin declaration
# - left click menu will pop up the button editor to configure inputs for the given pin

func _gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.is_released():
		match event.button_index:
			MOUSE_BUTTON_LEFT: # or MOUSE_BUTTON_LEFT in Godot 4
				_on_left_click()
			MOUSE_BUTTON_RIGHT: # or MOUSE_BUTTON_RIGHT in Godot 4
				_on_right_click()

func _on_left_click() -> void:
	var input_editor : InputEditor = input_editor.instantiate()
	
	input_editor.pin_name = pin.pin_name
	input_editor.initialize_ui_with_data(input_mappings)
	add_child(input_editor)
	input_editor.popup_centered() #(Rect2i(get_global_mouse_position(), Vector2i(1, 1)))
	
	input_editor.popup_hide.connect(func():
		input_editor.return_data_from_ui(input_mappings) 
		input_editor.queue_free()
	)

func _on_right_click() -> void:
	var pin_editor : PinEditor = pin_editor.instantiate()
	pin_editor.set_pin(pin)
	
	add_child(pin_editor)
	pin_editor.popup(Rect2i(get_global_mouse_position(), Vector2i(1, 1)))
	
	pin_editor.popup_hide.connect(func():
		pin_editor.fill_pin_data(pin)
		pin_editor.queue_free()
	)
