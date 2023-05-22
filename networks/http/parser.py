from enum import Enum, auto
import sys
import io


class HttpState(Enum):
    START = auto()
    HEADERS = auto()
    BODY = auto()
    END = auto()


class HttpRequest(object):
    def __init__(self) -> None:
        self.headers = {}
        self.residual = b""
        self.state = HttpState.START
        self.body = b""

    def parse(self, data):
        bs = io.BytesIO(self.residual + data)

        if self.state is HttpState.START:
            request_line = bs.readline()
            if request_line[-1:] != b"\n":
                self.residual = request_line
                return
            self.method, self.uri, self.version = request_line.rstrip().split(b" ")
            self.state = HttpState.HEADERS

        if self.state is HttpState.HEADERS:
            while True:
                field_line = bs.readline()
                if not field_line:
                    break
                if field_line[-1:] != b"\n":
                    self.residual = field_line
                    return

                if field_line == b"\r\n" or field_line == b"\n":
                    if self.method == b"GET":
                        self.state = HttpState.END
                    else:
                        self.state = HttpState.BODY
                    break
                field_name, field_value = field_line.rstrip().split(b": ")
                self.headers[field_name.lower()] = field_value
        if self.state is HttpState.BODY:
            self.body += bs.read()
