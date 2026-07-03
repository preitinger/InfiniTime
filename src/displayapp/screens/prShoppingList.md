# Dateien

## Arbeitsdateien

 Datei                    | Typ          | Bedeutung            
--------------------------|--------------|-------------------------
 /shoppingList-done.bin   | Arbeitsdatei | done- und skipped-Bits für DoneState   
 /shoppingList-id.bin     | Arbeitsdatei | Zwar "Arbeitsdatei", aber liegt nur rum bis sie wieder über shoppingList.tmp in shoppingList.ext.out gepackt wird. Das Handy prüft damit, ob die importierten Daten von der richtigen Uhr sind.
 /shoppingList-amount.bin | Arbeitsdatei | je Item 3 Bits für den Skalar der zu kaufenden Menge
  /shoppingList.txt       | Arbeitsdatei | für TextWindow            
 /shoppingList.ext.tmp    | temporär     | Nur für den schreibenden Bluetooth-Transfer
 /shoppingList.ext.in     | Import       | Künftige Inhalte von /shoppingList-id.bin, /shoppingList-amount.bin und /shoppingList.txt konkateniert
 /shoppingList.tmp        | temporär     | Zum Konkatenieren der obigen 3 Dateien für den Export
 /shoppingList.ext.out    | Export       | Für den lesenden Bluetooth-Transfer 
