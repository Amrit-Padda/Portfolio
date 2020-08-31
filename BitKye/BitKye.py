from src.app import create_app
from src.extensions import socketio


app = create_app()

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=80, cors_allowed_origins="*")