package com.rraptor.pult.io;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import org.kabeja.dxf.DXFConstants;
import org.kabeja.dxf.DXFDocument;
import org.kabeja.dxf.DXFLayer;
import org.kabeja.dxf.DXFLine;
import org.kabeja.parser.DXFParser;
import org.kabeja.parser.ParseException;
import org.kabeja.parser.Parser;
import org.kabeja.parser.ParserBuilder;

import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

/**
 * Work with dxf files with kabeja library.
 * http://kabeja.sourceforge.net/index.html
 * 
 * Example here:
 * http://stackoverflow.com/questions/6740736/reading-a-dxf-file-with-java
 * http://kabeja.sourceforge.net/docs/devel/embedding.html
 * 
 * Download binary release, take kabeja-0.4.jar from lib folder, all other jars
 * are not necessary and some of them with fail to compile for android.
 * 
 * @author Anton Moiseev
 * 
 */
public class DxfLoader {

    /**
     * Загрузить линии из файла в формате DXF.
     * 
     * @param input
     * @return
     * @throws ParseException
     */
    public static List<Line2D> getLines(final InputStream input)
            throws ParseException {
        final Parser parser = ParserBuilder.createDefaultParser();
        parser.parse(input, DXFParser.DEFAULT_ENCODING);
        final DXFDocument doc = parser.getDocument();
        final DXFLayer layer = doc.getDXFLayer("Layer_1");
        final List<DXFLine> lines = layer
                .getDXFEntities(DXFConstants.ENTITY_TYPE_LINE);

        final List<Line2D> lines2D = new ArrayList<Line2D>();
        if (lines != null) {
            for (DXFLine line : lines) {
                lines2D.add(new Line2D(new Point2D(line.getStartPoint().getX(),
                        line.getStartPoint().getY()), new Point2D(line
                        .getEndPoint().getX(), line.getEndPoint().getY())));
            }
        }
        return lines2D;
    }

    public static InputStream getTestDxfFile() throws IOException {
        return DxfLoader.class.getResource("rraptor_logo1.dxf").openStream();
    }
}
