# Program
- **src/hello_ffmpeg.cpp:** Read mp4 video and display its metadata.

# Prerequisite
```bash
sudo apt install libavformat-dev
```

# Notions about video processing
- **Demuxing:** Split individual streams (video, audio, subtitles...) and send them to their respective decoders.
- **Muxing:** Encapsulating multiple streams (video, audio...) into the same container (e.g. avi).
- **Bitrate:** Number of bits processed / time unit (second). Affects the audio/video quality.
    - Video size = video bitrate * duration.
    - Audio size = audio bitrate * duration.
- **Framerate:** Frames per second in the video (FPS).
- **Container:** e.g. AVI, MP4
    - Audio stream: e.g. encoded by MP3 codec.
    - Video stream: e.g. encoded by DivX codec
- **Processing chain:**
    - Video   -> *Demuxer* -> Packets (for each stream)
    - Packets -> *Decoder* -> Frames (i.e. decoded data)
    - Frames  -> *Encoder* -> Packets (i.e. encoded data)
    - Packets -> *Muxer*   -> Video output


# ffmpeg
- **libavformat:** muxing, demuxing.
- **libavcodec:** coding, decoding.


# Assets
- Big Buck Bunny: [big-buck-bunny]

[big-buck-bunny]: https://github.com/leandromoreira/ffmpeg-libav-tutorial/blob/master/small_bunny_1080p_60fps.mp4


# Tutorials for ffmpeg
- Excellent up-to-date tutorial: [leandromoreira/ffmpeg-libav-tutorial]
- [mpenkov/ffmpeg-tutorial]
- Examples: [leixiaohua1020]

[leandromoreira/ffmpeg-libav-tutorial]: https://github.com/leandromoreira/ffmpeg-libav-tutorial
[mpenkov/ffmpeg-tutorial]: https://github.com/mpenkov/ffmpeg-tutorial
[leixiaohua1020]: http://leixiaohua1020.github.io/#ffmpeg-development-examples
