extends PopupPanel
class_name PinEditor

func set_pin(pin : PinDeclaration) -> void:
	%PinInput.text = Enums.pin_to_text(pin.pin_name)
	%AnalogInput.button_pressed = pin.analog
	%InactiveValueInput.value = pin.inactive_value

func fill_pin_data(return_pin : PinDeclaration) -> void:
	return_pin.pin_name = Enums.text_to_pin(%PinInput.text)
	return_pin.analog = %AnalogInput.button_pressed
	return_pin.inactive_value = %InactiveValueInput.value
