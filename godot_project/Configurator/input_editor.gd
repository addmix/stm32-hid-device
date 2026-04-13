extends PopupPanel
class_name InputEditor

const mapping_entry = preload("res://Configurator/input_mapping_entry.tscn")

var pin_name : int = Enums.Pins.NC

func _ready() -> void:
	$ScrollContainer/VBoxContainer/InputMappingEntries/InputMappingEntry.queue_free()

func _enter_tree() -> void:
	$ScrollContainer/VBoxContainer/Header/Label2.text = Enums.pin_to_text(pin_name)

func initialize_ui_with_data(mappings : Array[InputMapping]) -> void:
	for mapping : InputMapping in mappings:
		var instance : Node = mapping_entry.instantiate()
		
		instance.fill_ui_from_data(mapping)
		
		$ScrollContainer/VBoxContainer/InputMappingEntries.add_child(instance)

func return_data_from_ui(mappings : Array[InputMapping]) -> void:
	
	for entry_id : int in $ScrollContainer/VBoxContainer/InputMappingEntries.get_child_count():
		var entry : Control = $ScrollContainer/VBoxContainer/InputMappingEntries.get_child(entry_id)
		var mapping : InputMapping
		if entry_id >= mappings.size():
			mapping = InputMapping.new()
			mappings.append(mapping)
		else:
			mapping = mappings[entry_id]
		
		mapping.pin_name = pin_name
		
		#fill mapping with data
		entry.fill_data_from_ui(mapping)
	
	if $ScrollContainer/VBoxContainer/InputMappingEntries.get_child_count() < mappings.size():
		for i in mappings.size() - $ScrollContainer/VBoxContainer/InputMappingEntries.get_child_count():
			#delete excess mappings
			mappings.pop_back()


func _on_add_input_pressed() -> void:
	$ScrollContainer/VBoxContainer/InputMappingEntries.add_child(mapping_entry.instantiate())



func _on_clear_confirmation_dialog_confirmed() -> void:
	for input_mapping in $ScrollContainer/VBoxContainer/InputMappingEntries.get_children():
		input_mapping.queue_free()
