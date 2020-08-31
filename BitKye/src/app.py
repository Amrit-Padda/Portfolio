from flask import Flask
from flask import Blueprint
from .config import Config
from src.main.hook import webhook_manager
from src.main.models import Queue, Pool

def create_app():
    app = Flask(__name__)
    app.config.from_object(Config)
    register_extensions(app)
    register_blueprints(app)
    webhook_manager
    return app

def register_extensions(app):
    from src.extensions import db
    from src.extensions import migrate
    from src.extensions import socketio
    from src.extensions import limiter
    db.init_app(app)
    migrate.init_app(app, db)
    socketio.init_app(app, cors_allowed_origins='*', ping_timeout=1000, ping_interval=50)
    limiter.init_app(app)

def register_blueprints(app):
    from src.views import view_bp
    app.register_blueprint(view_bp)

