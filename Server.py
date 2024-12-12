from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/api/keystrokes', methods=['POST'])
def receive_keystrokes():
    
    data = request.get_json()

    if 'keystroke' in data:
        keystroke = data['keystroke']
        print(f"Received keystroke: {keystroke}")

        return jsonify({"status": "success", "message": "Keystroke received"}), 200
    else:
        return jsonify({"status": "error", "message": "Keystroke not found"}), 400

if __name__ == '__main__':
    app.run(debug=True, port=5000)