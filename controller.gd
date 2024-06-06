extends Control

@onready var velocity_label: Label = $Velocity
@onready var http_request: HTTPRequest = $HTTPRequest

var dir: Vector3 = Vector3.ZERO
var velocity: Vector3 = Vector3.ZERO
var acch := 128
var accv := 64
var updated := false  # Flag to indicate whether velocity has been updated

func _input(event):
	if event is InputEventKey:
		updated = true  # Set the flag to true when input is detected and processed
		var lvl = Input.get_axis("ui_up", "ui_down")
		var vec = Input.get_vector("left", "right", "forward", "backward")
		dir = Vector3(vec.x, lvl, vec.y)

func _physics_process(delta: float) -> void:
	var prev_velocity = velocity
	velocity.x = clamp(velocity.x + dir.x * acch * delta, -512, 512) if dir.x != 0 else 0
	velocity.y = clamp(velocity.y + dir.y * accv * delta, -512, 512) if dir.y != 0 else 0
	velocity.z = clamp(velocity.z + dir.z * acch * delta, -512, 512) if dir.z != 0 else 0

	velocity_label.text = str(velocity)
	
	if (updated or velocity != prev_velocity) and $Timer.is_stopped():
		send_velocity_http_request(velocity)
		updated = false

func send_velocity_http_request(velocity):
	var data = {
		"velocity": {
			"x": int(velocity.x),
			"y": int(velocity.y),
			"z": int(velocity.z)
		}
	}
	var data_json = JSON.stringify(data)
	if http_request.is_processing():
		print("HTTPRequest is still processing another request.")
		return
	var headers = ["Content-Type: application/json"]
	var error = http_request.request("http://localhost:8743", headers, HTTPClient.METHOD_POST, data_json)
	if error != OK and error != ERR_BUSY:
		print(error)
	$Timer.start()
