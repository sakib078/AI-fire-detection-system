from ultralytics import YOLO
import cv2
import os

# Load the fire/smoke-specific YOLOv8 model
model = YOLO("fire-and-smoke-detection-2/weights/best.pt")  # Adjust path to your .pt file

def detect_fire_smoke(image_path):
    results = model(image_path)
    img = cv2.imread(image_path)
    fire_or_smoke_detected = False
    for r in results:
        boxes = r.boxes
        for box in boxes:
            label = model.names[int(box.cls)]
            if label in ["fire", "smoke"]:
                fire_or_smoke_detected = True
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                cv2.rectangle(img, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.putText(img, f"{label} {box.conf:.2f}", (x1, y1-10), 
                            cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)
    output_path = os.path.join(os.getcwd(), "detected_image.jpg")
    cv2.imwrite(output_path, img)
    return output_path, "Fire/Smoke Detected" if fire_or_smoke_detected else "No Fire/Smoke"

if __name__ == "__main__":
    image_path = os.path.join("test_images", "WIN_20250408_21_19_09_Pro.jpg")
    output, message = detect_fire_smoke(image_path)
    print(message)