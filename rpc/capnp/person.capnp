@0xbf811a3d36d4deec;

struct Person {
  id @0 :UInt32;
  name @1 :Text;
  email @2 :Text;
  phones @3 :List(PhoneNumber);
  address @8: Text;
  nationality @9: Text;
  bitrthday @10: Text;
  creationDate @11: Text;
  updateDate @12: Text;
  selfIntroduction @13: Text;

  struct PhoneNumber {
    number @0 :UInt32;
    type @1 :Type;

    enum Type {
      mobile @0;
      home @1;
      work @2;
    }
  }
}
