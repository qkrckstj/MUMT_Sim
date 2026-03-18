import socket
import json

UDP_IP = "127.0.0.1"
CONTROL_PORT = 5005
STATE_PORT = 5006

# Unreal로 명령 보내는 소켓
send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Unreal에서 상태 받는 소켓
recv_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
recv_sock.bind(("0.0.0.0", STATE_PORT))

print("Closed-loop controller started")

# -----------------------------
# 0. 초기 명령 1번 송신
# -----------------------------
roll = 0.0
pitch = 0.0
yaw = 0.0
throttle = 0.8

msg = f"{roll},{pitch},{yaw},{throttle}"
send_sock.sendto(msg.encode("utf-8"), (UDP_IP, CONTROL_PORT))
print("initial sent:", msg)

# -----------------------------
# 1. 상태 수신 -> 다음 명령 계산 -> 송신
# -----------------------------
while True:
    data, addr = recv_sock.recvfrom(4096)
    raw_msg = data.decode("utf-8")
    print("state raw:", raw_msg)

    try:
        state = json.loads(raw_msg)
    except Exception as e:
        print("JSON parse error:", e)
        continue

    # Unreal 새 코드 기준
    speed_cmps = state["speed"]
    speed_mps = state["speed_mps"]
    speed_kph = state["speed_kph"]

    pitch_state = state["pitch"]
    roll_state = state["roll"]
    yaw_state = state["yaw"]
    x = state["x"]
    y = state["y"]
    z = state["z"]

    print(
        f"speed={speed_cmps:.3f} cm/s, "
        f"{speed_mps:.3f} m/s, "
        f"{speed_kph:.3f} km/h | "
        f"pitch={pitch_state:.3f}, roll={roll_state:.3f}, yaw={yaw_state:.3f}"
    )

    # -----------------------------
    # 다음 제어 명령 계산
    # -----------------------------
    target_pitch = 5.0

    # 속도 기준 throttle
    if speed_mps < 150.0:
        throttle = 1.0
    else:
        throttle = 0.6

    # pitch 오차 기반 제어
    pitch_error = target_pitch - pitch_state
    pitch_cmd = pitch_error * 0.1

    # 제한
    if pitch_cmd > 1.0:
        pitch_cmd = 1.0
    elif pitch_cmd < -1.0:
        pitch_cmd = -1.0

    # 아까 테스트 결과 기준: pitch 축 반대로 적용
    pitch = -pitch_cmd

    roll = 0.0
    yaw = 0.0

    msg = f"{roll},{pitch},{yaw},{throttle}"
    send_sock.sendto(msg.encode("utf-8"), (UDP_IP, CONTROL_PORT))
    print("next sent:", msg)