package edu.wpi.grip.ui.codegeneration.tools;

import static org.junit.Assert.fail;

import java.lang.reflect.InvocationTargetException;

import org.opencv.core.Point;

public class JavaLine extends TestLine {

  private Object line;
  
  public JavaLine(Object line){
    this.line = line;
  }
  @Override
  public double getLength() {
    try {
      return (double) line.getClass().getMethod("length").invoke(line);
    } catch (NoSuchMethodException | SecurityException
        | IllegalAccessException | IllegalArgumentException | InvocationTargetException e) {
      e.printStackTrace();
      fail("length is not valid for class " + line.getClass().getSimpleName());
      return 0.0;
    }
  }

  @Override
  public double getAngle() {
    try {
      return (double) line.getClass().getMethod("angle").invoke(line);
    } catch (NoSuchMethodException | SecurityException
        | IllegalAccessException | IllegalArgumentException | InvocationTargetException e) {
      e.printStackTrace();
      fail("angle is not valid for class " + line.getClass().getSimpleName());
      return 0.0;
    }
  }
  @Override
  public Point getPoint1() {
    // TODO Auto-generated method stub
    return null;
  }
  @Override
  public Point getPoint2() {
    // TODO Auto-generated method stub
    return null;
  }

}