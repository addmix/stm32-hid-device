extends VBoxContainer

func fill_ui_from_data(mapping : InputMapping) -> void:
	%ReportDevice.select(mapping.input_type)
	%InputID.text = Enums.input_id_to_text(mapping.input_id, mapping.input_type) #TODO: maybe this conversion logic should be a function in the InputMapping class
	%Inverted.button_pressed = mapping.invert

func fill_data_from_ui(return_mapping : InputMapping) -> void:
	return_mapping.input_type = %ReportDevice.get_selected_id()
	return_mapping.input_id = Enums.text_to_input_id(%InputID.text, return_mapping.input_type) #TODO: maybe this conversion logic should be a function in the InputMapping class
	return_mapping.invert = %Inverted.button_pressed


func _on_delete_button_pressed() -> void:
	queue_free()
