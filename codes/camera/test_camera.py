import cv2

# Open the specific webcam by device index
capture = cv2.VideoCapture('/dev/v4l/by-id/usb-lihappe8_Corp._USB_2.0_Camera-video-index0')

if not capture.isOpened():
    print("Error: Could not open webcam.")
    exit()

while True:
    ret, frame = capture.read()
    if not ret:
        print("Error: Failed to capture frame.")
        break
    
    # Convert frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Apply Canny edge detection
    edges = cv2.Canny(gray, 100, 200)
    
    # Display the edge-detected frame
    cv2.imshow('Canny Edge Detection', edges)
    
    # Exit when 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the webcam and close windows
capture.release()
cv2.destroyAllWindows()

"""
import cv2

# Open the webcam (0 for default camera)
capture = cv2.VideoCapture("/dev/v4l/by-id/usb-lihappe8_Corp._USB_2.0_Camera-video-index0")


if not capture.isOpened():
    print("Error: Could not open webcam.")
    exit()

while True:
    ret, frame = capture.read()
    if not ret:
        print("Error: Failed to capture frame.")
        break
    
    # Display the frame
    cv2.imshow('Webcam Feed', frame)
    
    # Exit when 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the webcam and close windows
capture.release()
cv2.destroyAllWindows()
"""
