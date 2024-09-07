#Program that loads the Astermodel, whichi is in best.pt, and opens a camera window so that it can be used in computer vision

import cv2 #We need open cv to open the camera
import torch #To load the yolo model
from ultralytics import YOLO #Also to load the yolo model

# Load the trained YOLO model
model = YOLO("C:/Users/david/OneDrive/IEEEOuter/runs/detect/train4/weights/best.pt")  # Path to the trained Astermodel (best weight chosen)

# Open the camera (default camera is usually at index 0)
cap = cv2.VideoCapture(1)

if not cap.isOpened():
    print("Error: Could not open video stream.")
    exit()

# Set video capture properties, 640 x 480 should be okay
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

# Loop to capture frames from the camera
while True:
    ret, frame = cap.read()  # Capture a single frame from the camera, this will be sent to the model
    
    if not ret:
        print("Error: Failed to capture frame.") #Lets us know that the camera isnt capturing
        break

    # Perform object detection on the current frame using the model
    results = model(frame)  # YOLO model takes a frame as input

    # Plot the bounding boxes and labels on the frame
    annotated_frame = results[0].plot()  # Annotate the frame with detections

    # Show the frame with detections
    cv2.imshow('Asteroid Detection', annotated_frame)

    # Exit the loop if 'q' is pressed, we can change keys later
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera and close any open windows
cap.release()
cv2.destroyAllWindows()
