
uniform sampler2D baseTexture;
uniform vec2 pointPositions[10];
uniform vec4 pointColors[10];
uniform float pointRadii[10];
uniform int lightTotal;
uniform float texSize;
uniform vec4 dark;

void main()
{
    vec4 final = vec4(0.0, 0.0, 0.0, 0.0);
    bool isDark = true;
    bool firstPass = true;

    // Dark comparison
    vec4 lower = texture2D(baseTexture, gl_TexCoord[0].xy);
    vec4 upper = dark;
    vec4 darkColor = vec4(0.0, 0.0, 0.0, upper.a);
    
    if (upper.r > 0.5) {
        darkColor.r = (1.0 - (1.0-lower.r) * (1.0-2.0*(upper.r-0.5)));
    } else {
        darkColor.r = lower.r * (2.0*upper.r);
    }

    if (upper.g > 0.5) {
        darkColor.g = (1.0 - (1.0-lower.g) * (1.0-2.0*(upper.g-0.5)));
    } else {
        darkColor.g = lower.g * (2.0*upper.g);
    }

    if (upper.b > 0.5) {
        darkColor.b = (1.0 - (1.0-lower.b) * (1.0-2.0*(upper.b-0.5)));
    } else {
        darkColor.b = lower.b * (2.0*upper.b);
    }

    for (int i = 0; i < lightTotal; ++i)
    {
        vec2 pl = vec2(pointPositions[i].x / texSize, (texSize - pointPositions[i].y) / texSize);
        float radius = pointRadii[i] / texSize;
        float dist = distance(pl, vec2(gl_TexCoord[0].xy));
        if (dist < radius)
        {
            lower = texture2D(baseTexture, gl_TexCoord[0].xy);
            float mod = 1.0 - dist / radius;
            upper = pointColors[i] * (1.0 - dist / radius);
            vec4 outColor = vec4(0.0, 0.0, 0.0, upper.a);
    
            if (upper.r > 0.5) {
                outColor.r = (1.0 - (1.0-lower.r) * (1.0-2.0*(upper.r-0.5)));
            } else {
                outColor.r = lower.r * (2.0*upper.r);
            }

            if (upper.g > 0.5) {
                outColor.g = (1.0 - (1.0-lower.g) * (1.0-2.0*(upper.g-0.5)));
            } else {
                outColor.g = lower.g * (2.0*upper.g);
            }

            if (upper.b > 0.5) {
                outColor.b = (1.0 - (1.0-lower.b) * (1.0-2.0*(upper.b-0.5)));
            } else {
                outColor.b = lower.b * (2.0*upper.b);
            }

            outColor.r = max(outColor.r, darkColor.r);
            outColor.g = max(outColor.g, darkColor.g);
            outColor.b = max(outColor.b, darkColor.b);

            if (!firstPass)
            { 
                final.r = max(outColor.r, final.r);
                final.g = max(outColor.g, final.g);
                final.b = max(outColor.b, final.b);
            }
            else
            {
                final = outColor;
                firstPass = false;
            }

            float a = 1.0;
            if (final.r == 0.0 && final.b == 0.0 && final.g == 0.0)
            {
                a = 0.0;
            }

            final.a = a;

            isDark = false;
        }
    }

    if (isDark)
    {
        float a = 1.0;
        if (darkColor.r == 0.0 && darkColor.b == 0.0 && darkColor.g == 0.0)
        {
            a = 0.0;
        }
        darkColor.a = a;
        final = darkColor;
    }
    
    gl_FragColor = final;
} 