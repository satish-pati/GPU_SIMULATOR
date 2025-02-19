import sys
import tkinter as tk
from tkinter import scrolledtext
from sim import Simulator
import threading
import time
class ConsoleRedirector:
    def __init__(self, text_widget):
        self.text_widget = text_widget

    def write(self, message):
        self.text_widget.config(state=tk.NORMAL)
        self.text_widget.insert(tk.END, message)
        self.text_widget.see(tk.END)
        self.text_widget.config(state=tk.DISABLED)

    def flush(self):
        pass  

class CodeEditorWindow:
    def __init__(self, master):
        self.master = master
        self.master.title("RISC-V Simulator")
        self.master.configure(bg="white")  
        self.master.columnconfigure(0, weight=1)
        self.master.rowconfigure(0, weight=3)
        self.master.rowconfigure(1, weight=2)
        self.master.rowconfigure(2, weight=0)
        self.master.rowconfigure(3, weight=0)
        self.create_widgets()
        sys.stdout = ConsoleRedirector(self.output_section)
        sys.stderr = ConsoleRedirector(self.output_section)

    def create_widgets(self):
        text_bg = "#FFFFFF" 
        text_fg = "#000000" 
        self.code_input_section = scrolledtext.ScrolledText(
            self.master, wrap=tk.WORD, bg=text_bg, fg=text_fg, insertbackground="black",height=15
        )
        self.code_input_section.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
        self.output_section = scrolledtext.ScrolledText(
            self.master, wrap=tk.WORD, state=tk.DISABLED, bg=text_bg, fg=text_fg, insertbackground="black",height=10
        )
        self.output_section.grid(row=1, column=0, padx=10, pady=10, sticky="nsew")
        self.button_frame = tk.Frame(self.master, bg="white")
        self.button_frame.grid(row=2, column=0, pady=5, sticky="ew")
        self.run_button = tk.Button(self.button_frame, text="Run", command=self.run_code, bg="#007ACC", fg="white")
        self.run_button.pack(pady=5)
        self.loader_label = tk.Label(self.master, text="", font=("Arial", 12), bg="white", fg="black")
        self.loader_label.grid(row=3, column=0, pady=5)

    def run_code(self):
        self.save_code()
        self.run_button.config(state=tk.DISABLED)
        self.loader_label.config(text="Running simulation... Please wait.")
        self.loader_label.update()
        threading.Thread(target=self.execute_simulator, daemon=True).start()

    def save_code(self):
        code = self.code_input_section.get("1.0", tk.END).strip()
        if not code:
            return
        with open("program.s", "w") as file:
            file.write(code)

    def execute_simulator(self):
        try:
            time.sleep(1)  
            simulator = Simulator("program.s")
            simulator.run()

        except Exception as e:
            print(f"Error: {str(e)}")

        finally:
            self.master.after(100, self.reset_ui)

    def reset_ui(self):
        self.loader_label.config(text="")  
        self.run_button.config(state=tk.NORMAL)

# Create and start the GUI
root = tk.Tk()
window = CodeEditorWindow(root)
root.mainloop()
