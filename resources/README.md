# Heightmap
- Heightmap downloaded from [Wikipedia][wikipedia-heightmap].

[wikipedia-heightmap]: https://en.wikipedia.org/wiki/Heightmap


# Splatmap
1. The splatmap was produced in Gimp from the heightmap based on thresholding & masks as explained in [this response][gimp-select-by-histogram].
2. The obtained regions were painted in Red/Green/Blue, then

See this [Youtube video][youtube-splatmap], where a splatmap was produced with Photoshop, to get inspired.

## Smooth transition in Splatmap
### Method1
The borders of the regions were expanded and painted progressively in intermediary colors, which result in smoother transitions (gradient effect) and less edgy borders between textures. This [website][gradient-website] was used to generate gradient colors between green-red and green-blue.

### Method2
This method offers better transitions between the different terrain textures and is quicker to produce.

1. Flatten the red, green, and blue layers.
2. Select by color, then grow the selection by 5 pixels, and apply a Gaussian blue (menu filters > blur) 1.5 x 1.5.

[gimp-select-by-histogram]: https://photo.stackexchange.com/a/4841/40855
[youtube-splatmap]: https://www.youtube.com/watch?v=U-c9FnsXT_s
[gradient-website]: https://colordesigner.io/gradient-generator
