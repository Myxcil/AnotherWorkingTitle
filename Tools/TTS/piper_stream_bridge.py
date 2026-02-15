import sys, json, base64
from piper import PiperVoice

MAX_CHUNK_BYTES = 4096  # ~93ms @ 22.05kHz mono int16

def emit(obj):
    sys.stdout.write(json.dumps(obj) + "\n")
    sys.stdout.flush()

def main():
    if len(sys.argv) < 2:
        emit({"type": "error", "id": -1, "error": "usage: piper_stream_bridge.py <model.onnx>"})
        return 2

    model_path = sys.argv[1]
    voice = PiperVoice.load(model_path, use_cuda=False)
    sr = int(voice.config.sample_rate)
    ch = 1
    emit({"type": "ready", "sr": sr})

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            req = json.loads(line)
            rid = int(req["id"])
            text = str(req["text"])

            emit({"type": "start", "id": rid, "sr": sr, "ch": ch})

            seq = 0
            for chunk in voice.synthesize(text):
                data = chunk.audio_int16_bytes
                for i in range(0, len(data), MAX_CHUNK_BYTES):
                    part = data[i:i+MAX_CHUNK_BYTES]
                    b64 = base64.b64encode(part).decode("ascii")
                    emit({"type": "chunk", "id": rid, "seq": seq, "b64": b64})
                    seq += 1

            emit({"type": "done", "id": rid})

        except Exception as e:
            rid = -1
            try:
                rid = int(req.get("id", -1))
            except Exception:
                pass
            emit({"type": "error", "id": rid, "error": str(e)})

    return 0

if __name__ == "__main__":
    raise SystemExit(main())
