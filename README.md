# **Video to ASCII Art**

Convert videos to ASCII movies with this command line tool.

<br>

## **Usage**

Build the C extensions:

```bash
./make.sh
```

Convert the video file to ASCII: (note that the file should be placed in the same directory as [converter.py](converter.py))

```bash
./converter.py video.mp4
```

Play the ASCII movie from the terminal. You have to reduce the font size in your terminal (Ctrl + -) and an appropriate font should be selected. I have included the [square.ttf](square.ttf) font in the repository.

```bash
./player.py video.ascii
```

