import io
import picamera
import logging
import socketserver
from threading import Condition
import threading
from http import server
import sys
import select

# 웹 페이지의 HTML 코드
PAGE = """\
<!DOCTYPE html>
<html>
<head>
	<title>FINDER</title>
	<style>
		.container {
			display: flex;
			flex-direction: column;
			align-items: center;
			justify-content: center;
			height: 100vh;
		}

		.heading {
			text-align: center;
		}
	</style>
</head>
<body>
	<div class="container">
		<h1 class="heading">FINDER CAM</h1>
		<img src="stream.mjpg" width="640" height="480" />
	</div>
</body>
</html>
"""

# 비디오 스트리밍을 위한 출력 클래스
class StreamingOutput(object):
    def __init__(self):
        self.frame = None
        self.buffer = io.BytesIO()
        self.condition = Condition()

    def write(self, buf):
        if buf.startswith(b'\xff\xd8'):
            # 새로운 프레임이 시작될 때마다 기존 버퍼를 비우고 클라이언트에게 프레임을 알림
            self.buffer.truncate()
            with self.condition:
                self.frame = self.buffer.getvalue()
                self.condition.notify_all()
            self.buffer.seek(0)
        return self.buffer.write(buf)

# 비디오 스트리밍 요청을 처리하는 핸들러 클래스
class StreamingHandler(server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            # 루트 경로로 접속하면 /index.html로 리디렉션
            self.send_response(301)
            self.send_header('Location', '/index.html')
            self.end_headers()
        elif self.path == '/index.html':
            # /index.html 경로로 접속하면 웹 페이지를 반환
            content = PAGE.encode('utf-8')
            self.send_response(200)
            self.send_header('Content-Type', 'text/html')
            self.send_header('Content-Length', len(content))
            self.end_headers()
            self.wfile.write(content)
        elif self.path == '/stream.mjpg':
            # /stream.mjpg 경로로 접속하면 MJPEG 스트리밍 제공
            self.send_response(200)
            self.send_header('Age', 0)
            self.send_header('Cache-Control', 'no-cache, private')
            self.send_header('Pragma', 'no-cache')
            self.send_header('Content-Type', 'multipart/x-mixed-replace; boundary=FRAME')
            self.end_headers()
            try:
                while True:
                    with output.condition:
                        output.condition.wait()
                        frame = output.frame
                    self.wfile.write(b'--FRAME\r\n')
                    self.send_header('Content-Type', 'image/jpeg')
                    self.send_header('Content-Length', len(frame))
                    self.end_headers()
                    self.wfile.write(frame)
                    self.wfile.write(b'\r\n')
                    # 'q' 키가 눌리면 종료
                    if select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
                        if sys.stdin.readline().strip() == 'q':
                            break
            except Exception as e:
                logging.warning(
                    'Removed streaming client %s: %s',
                    self.client_address, str(e))
        else:
            # 올바르지 않은 경로일 경우 404 에러 반환
            self.send_error(404)
            self.end_headers()

# 스트리밍 서버 클래스
class StreamingServer(socketserver.ThreadingMixIn, server.HTTPServer):
    allow_reuse_address = True
    daemon_threads = True

# Raspberry Pi 카메라 초기화
with picamera.PiCamera(resolution='640x480', framerate=24) as camera:
    # 비디오 스트리밍을 위한 출력 객체 생성
    output = StreamingOutput()
    # 카메라 캡처를 출력 객체로 전달하여 스트리밍 시작
    camera.start_recording(output, format='mjpeg')
    try:
        address = ('', 8000)
        server = StreamingServer(address, StreamingHandler)
        print("Streaming server started.")
        # 스트리밍 서버 스레드 시작
        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.start()

        while True:
            # 'q' 키를 누를 때까지 계속 실행
            if select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
                if sys.stdin.readline().strip() == 'q':
                    break

        print("Stopping streaming server.")
        # 스트리밍 서버 종료
        server.shutdown()
        server.server_close()
        server_thread.join()

    finally:
        # 카메라 캡처 중지
        camera.stop_recording()
