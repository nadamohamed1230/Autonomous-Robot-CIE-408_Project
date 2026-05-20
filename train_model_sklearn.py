import os
import pandas as pd
import numpy as np
from sklearn.neural_network import MLPClassifier
from skl2onnx import to_onnx
from skl2onnx.common.data_types import FloatTensorType

# ── 1. LOAD AND CLEAN DATA ───────────────────────────────────────────────────
CSV_FILE = "ai_training_data.csv"
MODEL_OUTPUT = "car_avoidance.onnx"

if not os.path.exists(CSV_FILE):
    print(f"[ERROR] Could not find {CSV_FILE}!")
    exit(1)

df = pd.read_csv(CSV_FILE)

# Only train on human maneuvers to keep the AI data pure
df = df[df['Source'] == 'HUMAN']

print(f"[INFO] Training on {len(df)} rows of custom driving data...")

# Extract features and scale them (0.0 to 1.0) matching your Pi main code
X = df[['Front', 'Back', 'Right']].values / 200.0
y = df['Command'].values

# ── 2. INITIALIZE LIGHTWEIGHT NEURAL NETWORK ──────────────────────────────────
# MLPClassifier is a Multi-Layer Perceptron (Neural Network) in Scikit-Learn
clf = MLPClassifier(
    hidden_layer_sizes=(64, 32), 
    activation='relu', 
    solver='adam', 
    max_iter=500, 
    random_state=42
)

print("[INFO] Training the brain layers...")
clf.fit(X, y)

# ── 3. EXPORT TO ONNX FORMAT ──────────────────────────────────────────────────
print(f"[INFO] Converting model to {MODEL_OUTPUT}...")

# Tell ONNX to expect 3 floating-point inputs (Front, Back, Right)
initial_type = [('input', FloatTensorType([None, 3]))]
onnx_model = to_onnx(clf, X[:1].astype(np.float32), initial_types=initial_type)

# Save the binary model file
with open(MODEL_OUTPUT, "wb") as f:
    f.write(onnx_model.SerializeToString())

print("[SUCCESS] AI Brain generated flawlessly!")