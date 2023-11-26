import subprocess as sp
import time
import requests
import socket

HOST = '127.0.0.1'
PORT = 8080

EXECUTABLE = "./httpd"
LOG_FILE = "log_conf.txt"
CONFIG_FILE = "./config.conf"

headers = {
    "Host": HOST
}

def spawn_httpd(httpd_exec: str, config_file: str, stdout_filename: str) -> sp.Popen:
    with open(stdout_filename, "w") as f:
        httpd_process = sp.Popen(
            [httpd_exec, config_file], stdout=f, stderr=sp.PIPE, bufsize=0
        )
        # Let your httpd start
        time.sleep(1)

        return httpd_process


def kill_httpd(httpd_process: sp.Popen) -> None:
    httpd_process.kill()


def test_get_index() -> None:
    httpd_process = spawn_httpd(EXECUTABLE, CONFIG_FILE, LOG_FILE)

    try:
        response = requests.get(f"http://{HOST}:{PORT}/index.html", headers)
        assert response.status_code == 200
        assert response.text == "Hello, World!"
    finally:
        kill_httpd(httpd_process)


def test_get_noindex() -> None:
    httpd_process = spawn_httpd(EXECUTABLE, CONFIG_FILE, LOG_FILE)

    try:
        response = requests.get(f"http://{HOST}:{PORT}/", headers)
        assert response.status_code == 200
        assert response.text == "Default File"
    finally:
        kill_httpd(httpd_process)


def test_get_noindex2() -> None:
    httpd_process = spawn_httpd(EXECUTABLE, CONFIG_FILE, LOG_FILE)

    try:
        response = requests.get(f"http://{HOST}:{PORT}", headers)
        assert response.status_code == 200
        assert response.text == "Default File"
    finally:
        kill_httpd(httpd_process)


def test_get_noFile() -> None:
    httpd_process = spawn_httpd(EXECUTABLE, CONFIG_FILE, LOG_FILE)

    try:
        response = requests.get(f"http://{HOST}:{PORT}/to", headers)
        assert response.status_code == 404
    finally:
        kill_httpd(httpd_process)


def test_get_noFile_default() -> None:
    httpd_process = spawn_httpd(EXECUTABLE, CONFIG_FILE, LOG_FILE)

    try:
        response = requests.get(f"http://{HOST}:{PORT}/test", headers)
        assert response.status_code == 404
    finally:
        kill_httpd(httpd_process)


def test_get_forbidden() -> None:
    httpd_process = spawn_httpd(EXECUTABLE, CONFIG_FILE, LOG_FILE)

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            request = f"GET /../ HTTP/1.1\r\n{HOST}: {PORT}\r\nHost: {HOST}:{PORT}\r\n\r\n"
            s.sendall(request.encode())

            response = b""
            while True:
                data = s.recv(4096)
                if not data:
                    break
                response += data

            assert "HTTP/1.1 403 Forbidden" in response.decode()
    finally:
        kill_httpd(httpd_process)




def test_get_noheader() -> None:
    httpd_process = spawn_httpd(EXECUTABLE, CONFIG_FILE, LOG_FILE)

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            request = f"GET /../ HTTP/1.1\r\n\r\n"
            s.sendall(request.encode())

            response = b""
            while True:
                data = s.recv(4096)
                if not data:
                    break
                response += data

        assert "HTTP/1.1 400 Bad Request" in response.decode()
    finally:
        kill_httpd(httpd_process)