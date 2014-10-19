package com.rraptor.pult.io;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;

import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

public class SimpleContourIO {
    /**
     * Загрузить контур из файла.
     * 
     * Простой формат, одна строка - одна линия в виде: X1 Y1 X2 Y2
     * 
     * @param input
     * @return
     * @throws IOException
     */
    public static List<Line2D> loadLines(final InputStream input)
            throws IOException, ParseException {
        final List<Line2D> lines2D = new ArrayList<Line2D>();

        final BufferedReader reader = new BufferedReader(new InputStreamReader(
                input));
        String readLine;
        int lineCounter = 0;
        while ((readLine = reader.readLine()) != null) {
            lineCounter++;
            readLine = readLine.trim();
            if (readLine.length() > 0) {
                final String[] lineParts = readLine.split(" ");
                if (lineParts.length == 4) {
                    lines2D.add(new Line2D(new Point2D(Double
                            .parseDouble(lineParts[0]), Double
                            .parseDouble(lineParts[1])), new Point2D(Double
                            .parseDouble(lineParts[2]), Double
                            .parseDouble(lineParts[3]))));
                } else {
                    throw new ParseException(
                            "Each line should be in format: X1 Y1 X2 Y2",
                            lineCounter);
                }
            }
        }

        return lines2D;
    }

    /**
     * Сохранить контур в текстовый файл.
     * 
     * Простой формат, одна строка - одна линия в виде: X1 Y1 X2 Y2
     * 
     * @param output
     * @param lines
     */
    public static void saveLines(final OutputStream output,
            final List<Line2D> lines) throws IOException {
        final OutputStreamWriter writer = new OutputStreamWriter(output);
        for (final Line2D line : lines) {
            writer.write("" + line.getStart().getX() + " "
                    + line.getStart().getY() + " " + line.getEnd().getX() + " "
                    + line.getEnd().getY() + "\n");
        }
        writer.close();
    }
}
