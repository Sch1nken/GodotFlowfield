extends Node

@onready var flowfield = Flowfield.new()

func _input(event):
	if Input.is_action_just_pressed("test"):
		print("Starting!")
		print(flowfield.is_valid())
		flowfield.calculate_intergration_field(Vector2i(0, 0))
		print(flowfield.is_valid())
		flowfield.set_map_size(Vector2(150, 150))
		print(flowfield.is_valid())
		flowfield.calculate_intergration_field(Vector2(0, 0))
		print(flowfield.is_valid())
		var a = flowfield.get_data()
		print("Done!")
