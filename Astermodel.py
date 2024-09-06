import os
from PIL import Image
import torch
from torch.utils.data import DataLoader, Dataset
from torchvision import transforms
from ultralytics import YOLO

# Custom Dataset class that inherits from PyTorch's Dataset
class CustomAsteroidDataset(Dataset):
    def __init__(self, image_dir: str, label_dir: str, transform=None):
        self.image_dir = image_dir
        self.label_dir = label_dir
        self.transform = transform
        self.image_files = [f for f in os.listdir(self.image_dir) if f.endswith(('.jpg', '.png', '.jpeg'))]  # Filter image files

    def __len__(self) -> int:
        '''Returns the total number of image files'''
        return len(self.image_files)

    def __getitem__(self, idx: int):
        # Load image
        img_name = self.image_files[idx]
        img_path = os.path.join(self.image_dir, img_name)
        image = Image.open(img_path).convert("RGB")
        
        # Load corresponding label
        label_name = img_name.replace('.jpg', '.txt').replace('.png', '.txt').replace('.jpeg', '.txt')
        label_path = os.path.join(self.label_dir, label_name)

        labels = []
        if os.path.exists(label_path):
            with open(label_path, 'r') as f:
                for line in f.readlines():
                    # Each line contains: class_id x_center y_center width height
                    labels.append(list(map(float, line.strip().split())))

        labels = torch.tensor(labels)  # Convert labels to a tensor

        # Apply any transformations to the image
        if self.transform:
            image = self.transform(image)

        return image, labels


# Please use CUDA if you have access to it, it makes things much faster
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# Set up data transformations (resize, tensor conversion)
transform = transforms.Compose([
    transforms.Resize((640, 640)),  # Resize images to 640x640 (common YOLO input size)
    transforms.ToTensor()  # Convert images to tensors
])

# Load the dataset using the new image and label directories
image_dir = "C:/Users/david/OneDrive/IEEEOuter/IEEE/Astermodel_mk1/train/images"
label_dir = "C:/Users/david/OneDrive/IEEEOuter/IEEE/Astermodel_mk1/train/labels"
train_dataset = CustomAsteroidDataset(image_dir=image_dir, label_dir=label_dir, transform=transform)

# Create DataLoader
train_loader = DataLoader(train_dataset, batch_size=16, shuffle=True)

# Initialize the YOLOv8 model
model = YOLO('yolov8n.pt')  # Use YOLOv8 with pre-trained weights
model = model.to(device)

# Set up an optimizer
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

# Set up our loss function
criterion = torch.nn.CrossEntropyLoss()

# The training loop
def train_model(num_epochs=50):  # More epochs might lead to better performance
    for epoch in range(num_epochs):
        model.train(data='C:/Users/david/OneDrive/IEEEOuter/IEEE/Astermodel_mk1/data.yaml', epochs=50) # Switch to training mode
        running_loss = 0.0

        for i, (images, labels) in enumerate(train_loader):
            images = images.to(device)
            labels = labels.to(device)

            optimizer.zero_grad()  # Zero the parameter gradients

            # Forward pass
            outputs = model(images)

            # Find our loss values
            loss = criterion(outputs, labels)
            running_loss += loss.item()

            # Backpropagation and optimization
            loss.backward()
            optimizer.step()

            print(f"Epoch [{epoch+1}/{num_epochs}], Batch [{i+1}/{len(train_loader)}], Loss: {loss.item():.4f}")

        print(f"Epoch [{epoch+1}/{num_epochs}], Average Loss: {running_loss/len(train_loader):.4f}")

# Run the training process
if __name__ == "__main__":
    train_model(num_epochs=50)  # We can adjust the number of epochs as needed
