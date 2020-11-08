#include <libavformat/avformat.h>

int main() {
  // open mp4 file & read its header
  AVFormatContext* context = NULL;
  if (avformat_open_input(&context, "assets/bbb.mp4", NULL, NULL) != 0) {
    printf("Failed to open stream\n");
  }

  // show video metadata
  printf("Input format name: %s\n", context->iformat->name);
  printf("Duration: %ld microseconds\n", context->duration);
  printf("Number of streams: %d\n", context->nb_streams);

  // free opened context
  avformat_close_input(&context);

  return 0;
}
