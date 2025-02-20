import sys 
import tkinter as tk
from tkinter import scrolledtext
from sim import Simulator
import threading
import time
import re


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
        self.master.configure(bg="#121212")  # Dark gray background

        # Maximize window while keeping close button
        self.master.state("zoomed")

        # Configure grid layout
        self.master.columnconfigure(0, weight=1)
        self.master.columnconfigure(1, weight=1)
        self.master.rowconfigure(0, weight=1)
        self.master.rowconfigure(1, weight=0)

        self.create_widgets()

        # Redirect console output to the output section
        sys.stdout = ConsoleRedirector(self.output_section)
        sys.stderr = ConsoleRedirector(self.output_section)

    def create_widgets(self):
        text_bg = "#1e1e1e"  # Darker gray for code input
        text_fg = "white"  # Default text color

        # Input Section (Left Side) - Code Editor
        self.code_input_section = scrolledtext.ScrolledText(
            self.master, wrap=tk.WORD, bg=text_bg, fg=text_fg, insertbackground="white",
            font=("Consolas", 14), undo=True
        )
        self.code_input_section.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
        self.code_input_section.bind("<KeyRelease>", self.syntax_highlighting)

        # Output Section (Right Side)
        self.output_section = scrolledtext.ScrolledText(
            self.master, wrap=tk.WORD, state=tk.DISABLED, bg=text_bg, fg="lightgray", insertbackground="white",
            font=("Consolas", 14)
        )
        self.output_section.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        # Button Frame (Bottom, Spanning Two Columns)
        self.button_frame = tk.Frame(self.master, bg="#121212")
        self.button_frame.grid(row=1, column=0, columnspan=2, pady=10, sticky="ew")

        # Run Button
        self.run_button = tk.Button(
            self.button_frame, text="▶ Run Code", command=self.run_code,
            bg="#007ACC", fg="white", font=("Arial", 14, "bold"),
            activebackground="#005F99", activeforeground="white",
            relief="flat", padx=15, pady=5
        )
        self.run_button.pack(pady=5)

        # Loader Label
        self.loader_label = tk.Label(self.master, text="", font=("Arial", 14), bg="#121212", fg="white")
        self.loader_label.grid(row=2, column=0, columnspan=2, pady=10)

    def syntax_highlighting(self, event=None):
        """
        Apply syntax highlighting to the code input section.
        """
        self.code_input_section.tag_remove("instr", "1.0", tk.END)
        self.code_input_section.tag_remove("register", "1.0", tk.END)
        self.code_input_section.tag_remove("number", "1.0", tk.END)
        self.code_input_section.tag_remove("comment", "1.0", tk.END)
        self.code_input_section.tag_remove("directive", "1.0", tk.END)  # Add directive tag

        code = self.code_input_section.get("1.0", tk.END)

        # Define colors for syntax elements
        instr_color = "#FFAA00"  # Orange for instructions
        register_color = "#22DD88"  # Green for registers
        number_color = "#FF6666"  # Red for numbers
        comment_color = "#888888"  # Gray for comments
        directive_color = "#22DD88"  # Green for `.word`

        # RISC-V instruction set (simplified)
        instructions = r"\b(add|addi|sub|mul|div|li|la|mv|lw|sw|beq|bne|j|jr|jal|jalr|ble|beq)\b"
        registers = r"\b(x[0-9]+|sp|gp|tp|fp|ra|t[0-6]|s[0-9]|a[0-7])\b"
        numbers = r"\b\d+\b"
        comments = r"#.*"
        directives = r"\b\.word\b"  # Match `.word` exactly

        self.highlight_pattern(instructions, "instr", instr_color)
        self.highlight_pattern(registers, "register", register_color)
        self.highlight_pattern(numbers, "number", number_color)
        self.highlight_pattern(comments, "comment", comment_color)
        self.highlight_pattern(directives, "directive", directive_color)
    def highlight_pattern(self, pattern, tag, color):
        """
        Highlight text matching a regex pattern.
        """
        start = "1.0"
        while True:
            match = re.search(pattern, self.code_input_section.get(start, tk.END), re.MULTILINE)
            if not match:
                break
            pos_start = f"{start}+{match.start()}c"
            pos_end = f"{start}+{match.end()}c"
            self.code_input_section.tag_add(tag, pos_start, pos_end)
            self.code_input_section.tag_config(tag, foreground=color)
            start = pos_end

    def run_code(self):
        """
        Run the code entered in the editor.
        """
        self.save_code()
        self.run_button.config(state=tk.DISABLED)
        self.loader_label.config(text="Running simulation... Please wait.")
        self.loader_label.update()
        threading.Thread(target=self.execute_simulator, daemon=True).start()

    def save_code(self):
        """
        Save the current code to a file.
        """
        code = self.code_input_section.get("1.0", tk.END).strip()
        if not code:
            return
        with open("program.s", "w") as file:
            file.write(code)

    def execute_simulator(self):
        """
        Execute the RISC-V simulator and capture output.
        """
        try:
            time.sleep(1)
            simulator = Simulator("program.s")
            simulator.run()
        except Exception as e:
            print(f"Error: {str(e)}")
        finally:
            self.master.after(100, self.reset_ui)

    def reset_ui(self):
        """
        Reset UI elements after execution.
        """
        self.loader_label.config(text="")
        self.run_button.config(state=tk.NORMAL)

# Create and start the GUI
root = tk.Tk()
window = CodeEditorWindow(root)
root.mainloop()
