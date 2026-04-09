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
	var input_editor : PopupPanel = input_editor.instantiate()
	
	add_child(input_editor)
	input_editor.popup(Rect2i(get_global_mouse_position(), Vector2i.ZERO))
	
	input_editor.popup_hide.connect(func():
		
		#TODO actually configure all the buttons
		
		input_editor.queue_free()
	)

func _on_right_click() -> void:
	var pin_editor : PinEditor = pin_editor.instantiate()
	pin_editor.set_pin(pin)
	
	add_child(pin_editor)
	pin_editor.popup(Rect2i(get_global_mouse_position(), Vector2i.ZERO))
	
	pin_editor.popup_hide.connect(func():
		pin_editor.fill_pin_data(pin)
		
		print(pin)
		pin_editor.queue_free()
	)
