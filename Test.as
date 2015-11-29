package
{

import flash.display.*;
import flash.text.*;

public class Test extends Sprite
{
  private var textField:TextField = new TextField();

  public function Test():void
  {
    textField.defaultTextFormat = new TextFormat("Arial", 60);
    textField.text = "Hello, World!";
    textField.autoSize = TextFieldAutoSize.LEFT;
    addChild(textField);
  }
}
}
