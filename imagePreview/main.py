import os
import zipfile
from PIL import Image, ImageTk
import tkinter as tk
from tkinter import messagebox
from io import BytesIO


def find_zip_files(directory):
    """Find all .zip files in the specified directory, sorted by name."""
    return sorted([os.path.join(directory, f) for f in os.listdir(directory) if f.lower().endswith('.zip')])


def extract_images_from_zip(zip_path):
    """Extract all images from a ZIP file, handling encoded file names."""
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        images = []
        for file_name in zip_ref.namelist():
            try:
                # Try decoding the file name from cp437 to utf-8
                file_name = file_name.encode('cp437').decode('utf-8')
            except UnicodeDecodeError:
                # Fallback to original file name if decoding fails
                pass

            # Check for supported image file extensions
            if file_name.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif', '.webp')):
                data = zip_ref.read(file_name)
                image = Image.open(BytesIO(data))
                images.append(image)
        return images


class ImageSlideshow:
    def __init__(self, root, zip_files, directory, interval=3):
        self.root = root
        self.directory = directory
        self.zip_files = zip_files
        self.current_zip_index = 0
        self.images = []
        self.index = 0
        self.interval = interval  # Auto-play interval in seconds
        self.is_paused = False  # Pause state control
        self.slideshow_id = None  # To store the ID of after, for canceling timers
        self.is_fullscreen = False  # Fullscreen state

        # Configure root layout for grid
        root.grid_rowconfigure(0, weight=1)
        root.grid_columnconfigure(0, weight=1)
        root.grid_columnconfigure(1, weight=0)

        # Main layout (for image display)
        self.main_frame = tk.Frame(root)
        self.main_frame.grid(row=0, column=0, sticky="nsew")

        # Sidebar layout (for file list and buttons)
        self.sidebar = tk.Frame(root, width=200, bg="lightgrey")
        self.sidebar.grid(row=0, column=1, sticky="ns")

        # Sidebar title
        tk.Label(self.sidebar, text="ZIP File List", bg="lightgrey", font=("Arial", 14)).pack(pady=10)

        # File list
        self.file_listbox = tk.Listbox(self.sidebar, selectmode=tk.SINGLE, font=("Arial", 12))
        self.file_listbox.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        self.update_file_listbox()

        # Bind double-click event to load the selected ZIP
        self.file_listbox.bind('<Double-Button-1>', self.load_selected_zip)

        # Load button
        tk.Button(self.sidebar, text="Load Selected ZIP", command=self.load_selected_zip).pack(pady=5, padx=10)

        # Delete button
        tk.Button(self.sidebar, text="Delete Selected ZIP", command=self.delete_selected_zip).pack(pady=5, padx=10)

        # Adjust interval input
        tk.Label(self.sidebar, text="Set Slide Interval (seconds):", bg="lightgrey", font=("Arial", 12)).pack(pady=5, padx=10)
        self.interval_entry = tk.Entry(self.sidebar, font=("Arial", 12))
        self.interval_entry.pack(pady=5, padx=10)
        self.interval_entry.insert(0, str(self.interval))  # Pre-fill with default interval
        tk.Button(self.sidebar, text="Set Interval", command=self.set_interval).pack(pady=5, padx=10)

        # Image display area
        self.label = tk.Label(self.main_frame)
        self.label.pack(expand=True, fill=tk.BOTH)

        # Display current ZIP file name
        self.zip_name_label = tk.Label(self.main_frame, text="", font=("Arial", 16), fg="blue")
        self.zip_name_label.pack(side=tk.TOP, pady=5)

        # Page number display
        self.page_label = tk.Label(self.main_frame, text="Page 0/0", font=("Arial", 12), fg="black")
        self.page_label.pack(side=tk.BOTTOM, pady=5)

        # Create a pause indicator label
        self.pause_label = tk.Label(
            self.main_frame,
            text="PAUSE",
            font=("Arial", 24, "bold"),  # 加粗字体
            fg="red",                   # 红色字体
            bg=None                     # 无背景
        )
        self.pause_label.place(relx=0.5, y=10, anchor="n")
        self.pause_label.place_forget()

        # Get screen width and height
        self.screen_width = root.winfo_screenwidth()
        self.screen_height = root.winfo_screenheight()

        # Load initial ZIP file
        self.load_zip(self.current_zip_index)

        # Bind keys for various actions
        self.root.bind('<Left>', self.previous_image)
        self.root.bind('<Right>', self.next_image)
        self.root.bind('<space>', self.toggle_pause)
        self.root.bind('<Return>', self.next_zip)
        self.root.bind('<BackSpace>', self.previous_zip)
        self.root.bind('<Delete>', self.delete_current_zip)
        self.root.bind('<F11>', self.toggle_fullscreen)  # Bind F11 for fullscreen toggle
        self.root.bind('<Escape>', self.exit_fullscreen)  # Bind Escape to exit fullscreen

        # Start auto-play
        self.start_slideshow()

    def update_file_listbox(self):
        """Update the file list."""
        self.file_listbox.delete(0, tk.END)
        self.zip_files = find_zip_files(self.directory)
        for zip_file in self.zip_files:
            self.file_listbox.insert(tk.END, os.path.basename(zip_file))

    def load_zip(self, index):
        """Load images from the specified ZIP file."""
        if 0 <= index < len(self.zip_files):
            zip_path = self.zip_files[index]
            self.images = extract_images_from_zip(zip_path)
            self.index = 0
            if not self.images:
                print(f"No images found in {zip_path}")
            else:
                print(f"Loaded images from {zip_path}")
            self.zip_name_label.config(text=f"{os.path.basename(zip_path)}")
            self.update_image()
            self.pause_slideshow()  # Automatically pause when a new ZIP is loaded

    def update_image(self):
        if not self.images:
            return

        # Update page number
        total_pages = len(self.images)
        current_page = self.index + 1
        self.page_label.config(text=f"Page {current_page}/{total_pages}")

        # Update image
        img = self.images[self.index]
        img_width, img_height = img.size
        max_width = int(self.screen_width * 0.9)
        max_height = int(self.screen_height * 0.9)

        if img_width > max_width or img_height > max_height:
            scaling_factor = min(max_width / img_width, max_height / img_height)
            new_width = int(img_width * scaling_factor)
            new_height = int(img_height * scaling_factor)
            img = img.resize((new_width, new_height), Image.ANTIALIAS)

        photo = ImageTk.PhotoImage(img)
        self.label.config(image=photo)
        self.label.image = photo

    def next_image(self, event=None):
        if self.images:
            self.index = (self.index + 1) % len(self.images)
            self.update_image()

    def previous_image(self, event=None):
        if self.images:
            self.index = (self.index - 1) % len(self.images)
            self.update_image()

    def toggle_pause(self, event=None):
        self.is_paused = not self.is_paused
        if self.is_paused:
            self.pause_label.place(relx=0.5, y=10, anchor="n")
            self.cancel_slideshow()
        else:
            self.pause_label.place_forget()
            self.start_slideshow()

    def pause_slideshow(self):
        """Pause the slideshow and show the pause indicator."""
        self.is_paused = True
        self.pause_label.place(relx=0.5, y=10, anchor="n")
        self.cancel_slideshow()

    def next_zip(self, event=None):
        """Load the next ZIP file."""
        self.cancel_slideshow()
        self.current_zip_index = (self.current_zip_index + 1) % len(self.zip_files)
        self.load_zip(self.current_zip_index)
        if not self.is_paused:
            self.start_slideshow()

    def previous_zip(self, event=None):
        """Load the previous ZIP file."""
        self.cancel_slideshow()
        self.current_zip_index = (self.current_zip_index - 1) % len(self.zip_files)
        self.load_zip(self.current_zip_index)
        if not self.is_paused:
            self.start_slideshow()

    def start_slideshow(self):
        self.cancel_slideshow()
        if not self.is_paused:
            self.slideshow_id = self.root.after(self.interval * 1000, self.slideshow_step)

    def slideshow_step(self):
        self.next_image()
        self.start_slideshow()

    def cancel_slideshow(self):
        if self.slideshow_id is not None:
            self.root.after_cancel(self.slideshow_id)
            self.slideshow_id = None

    def load_selected_zip(self, event=None):
        """Load the selected ZIP file."""
        selection = self.file_listbox.curselection()
        if selection:
            self.current_zip_index = selection[0]
            self.load_zip(self.current_zip_index)

    def delete_selected_zip(self):
        """Delete the selected ZIP file."""
        selection = self.file_listbox.curselection()
        if selection:
            selected_index = selection[0]
            zip_path = self.zip_files[selected_index]
            self.confirm_and_delete(zip_path)

    def delete_current_zip(self, event=None):
        """Delete the currently loaded ZIP file."""
        if self.zip_files and 0 <= self.current_zip_index < len(self.zip_files):
            zip_path = self.zip_files[self.current_zip_index]
            self.confirm_and_delete(zip_path)

    def confirm_and_delete(self, zip_path):
        """Confirm deletion and remove the file."""
        confirm = messagebox.askyesno("Confirm Deletion", f"Are you sure you want to delete {os.path.basename(zip_path)}?")
        if confirm:
            os.remove(zip_path)
            print(f"Deleted {zip_path}")
            self.update_file_listbox()

            # Update current ZIP index and load next file if available
            self.zip_files = find_zip_files(self.directory)
            if self.zip_files:
                self.current_zip_index %= len(self.zip_files)
                self.load_zip(self.current_zip_index)
            else:
                self.images = []
                self.update_image()
                messagebox.showinfo("No ZIP files", "No more ZIP files available.")

    def toggle_fullscreen(self, event=None):
        """Toggle fullscreen mode."""
        self.is_fullscreen = not self.is_fullscreen
        self.root.attributes("-fullscreen", self.is_fullscreen)

    def exit_fullscreen(self, event=None):
        """Exit fullscreen mode."""
        self.is_fullscreen = False
        self.root.attributes("-fullscreen", False)

    def set_interval(self):
        """Set the slideshow interval based on user input."""
        try:
            new_interval = int(self.interval_entry.get())
            if new_interval > 0:
                self.interval = new_interval
                messagebox.showinfo("Success", f"Slideshow interval set to {new_interval} seconds")
            else:
                raise ValueError
        except ValueError:
            messagebox.showerror("Error", "Please enter a valid positive integer.")


if __name__ == '__main__':
    directory = '/mnt/d/Motrix-1.8.19-ia32-win/Downloads/comics/'

    zip_files = find_zip_files(directory)

    if zip_files:
        root = tk.Tk()
        root.title("ZIP Image Viewer")

        slideshow = ImageSlideshow(root, zip_files, directory, interval=5)

        root.mainloop()
    else:
        print("No zip files found in the directory.")
