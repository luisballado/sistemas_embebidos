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
    
    key = cv2.waitKey(1) & 0xFF
    
    # If a number key (0-9) is pressed, save the image
    if ord('0') <= key <= ord('9'):
        filename = f'photo_{chr(key)}.png'
        cv2.imwrite(filename, frame)
        print(f"Photo saved as {filename}")
    
    # Exit when 'q' is pressed
    if key == ord('q'):
        break

# Release the webcam and close windows
capture.release()
cv2.destroyAllWindows()
