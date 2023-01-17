using Capnp;
using Capnp.Rpc;
using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace CapnpGen
{
    [System.CodeDom.Compiler.GeneratedCode("capnpc-csharp", "1.3.0.0"), TypeId(0xa03f905da7ffadb0UL)]
    public class Person : ICapnpSerializable
    {
        public const UInt64 typeId = 0xa03f905da7ffadb0UL;
        void ICapnpSerializable.Deserialize(DeserializerState arg_)
        {
            var reader = READER.create(arg_);
            Id = reader.Id;
            Name = reader.Name;
            Email = reader.Email;
            Phones = reader.Phones?.ToReadOnlyList(_ => CapnpSerializable.Create<CapnpGen.Person.PhoneNumber>(_));
            Scores = reader.Scores;
            Address = reader.Address;
            Nationality = reader.Nationality;
            Bitrthday = reader.Bitrthday;
            CreationDate = reader.CreationDate;
            UpdateDate = reader.UpdateDate;
            SelfIntroduction = reader.SelfIntroduction;
            applyDefaults();
        }

        public void serialize(WRITER writer)
        {
            writer.Id = Id;
            writer.Name = Name;
            writer.Email = Email;
            writer.Phones.Init(Phones, (_s1, _v1) => _v1?.serialize(_s1));
            writer.Scores.Init(Scores);
            writer.Address = Address;
            writer.Nationality = Nationality;
            writer.Bitrthday = Bitrthday;
            writer.CreationDate = CreationDate;
            writer.UpdateDate = UpdateDate;
            writer.SelfIntroduction = SelfIntroduction;
        }

        void ICapnpSerializable.Serialize(SerializerState arg_)
        {
            serialize(arg_.Rewrap<WRITER>());
        }

        public void applyDefaults()
        {
        }

        public uint Id
        {
            get;
            set;
        }

        public string Name
        {
            get;
            set;
        }

        public string Email
        {
            get;
            set;
        }

        public IReadOnlyList<CapnpGen.Person.PhoneNumber> Phones
        {
            get;
            set;
        }

        public IReadOnlyList<float> Scores
        {
            get;
            set;
        }

        public string Address
        {
            get;
            set;
        }

        public string Nationality
        {
            get;
            set;
        }

        public string Bitrthday
        {
            get;
            set;
        }

        public string CreationDate
        {
            get;
            set;
        }

        public string UpdateDate
        {
            get;
            set;
        }

        public string SelfIntroduction
        {
            get;
            set;
        }

        public struct READER
        {
            readonly DeserializerState ctx;
            public READER(DeserializerState ctx)
            {
                this.ctx = ctx;
            }

            public static READER create(DeserializerState ctx) => new READER(ctx);
            public static implicit operator DeserializerState(READER reader) => reader.ctx;
            public static implicit operator READER(DeserializerState ctx) => new READER(ctx);
            public uint Id => ctx.ReadDataUInt(0UL, 0U);
            public string Name => ctx.ReadText(0, null);
            public string Email => ctx.ReadText(1, null);
            public IReadOnlyList<CapnpGen.Person.PhoneNumber.READER> Phones => ctx.ReadList(2).Cast(CapnpGen.Person.PhoneNumber.READER.create);
            public IReadOnlyList<float> Scores => ctx.ReadList(3).CastFloat();
            public string Address => ctx.ReadText(4, null);
            public string Nationality => ctx.ReadText(5, null);
            public string Bitrthday => ctx.ReadText(6, null);
            public string CreationDate => ctx.ReadText(7, null);
            public string UpdateDate => ctx.ReadText(8, null);
            public string SelfIntroduction => ctx.ReadText(9, null);
        }

        public class WRITER : SerializerState
        {
            public WRITER()
            {
                this.SetStruct(1, 10);
            }

            public uint Id
            {
                get => this.ReadDataUInt(0UL, 0U);
                set => this.WriteData(0UL, value, 0U);
            }

            public string Name
            {
                get => this.ReadText(0, null);
                set => this.WriteText(0, value, null);
            }

            public string Email
            {
                get => this.ReadText(1, null);
                set => this.WriteText(1, value, null);
            }

            public ListOfStructsSerializer<CapnpGen.Person.PhoneNumber.WRITER> Phones
            {
                get => BuildPointer<ListOfStructsSerializer<CapnpGen.Person.PhoneNumber.WRITER>>(2);
                set => Link(2, value);
            }

            public ListOfPrimitivesSerializer<float> Scores
            {
                get => BuildPointer<ListOfPrimitivesSerializer<float>>(3);
                set => Link(3, value);
            }

            public string Address
            {
                get => this.ReadText(4, null);
                set => this.WriteText(4, value, null);
            }

            public string Nationality
            {
                get => this.ReadText(5, null);
                set => this.WriteText(5, value, null);
            }

            public string Bitrthday
            {
                get => this.ReadText(6, null);
                set => this.WriteText(6, value, null);
            }

            public string CreationDate
            {
                get => this.ReadText(7, null);
                set => this.WriteText(7, value, null);
            }

            public string UpdateDate
            {
                get => this.ReadText(8, null);
                set => this.WriteText(8, value, null);
            }

            public string SelfIntroduction
            {
                get => this.ReadText(9, null);
                set => this.WriteText(9, value, null);
            }
        }

        [System.CodeDom.Compiler.GeneratedCode("capnpc-csharp", "1.3.0.0"), TypeId(0xab91e0d66405b731UL)]
        public class PhoneNumber : ICapnpSerializable
        {
            public const UInt64 typeId = 0xab91e0d66405b731UL;
            void ICapnpSerializable.Deserialize(DeserializerState arg_)
            {
                var reader = READER.create(arg_);
                Number = reader.Number;
                TheType = reader.TheType;
                applyDefaults();
            }

            public void serialize(WRITER writer)
            {
                writer.Number = Number;
                writer.TheType = TheType;
            }

            void ICapnpSerializable.Serialize(SerializerState arg_)
            {
                serialize(arg_.Rewrap<WRITER>());
            }

            public void applyDefaults()
            {
            }

            public uint Number
            {
                get;
                set;
            }

            public CapnpGen.Person.PhoneNumber.Type TheType
            {
                get;
                set;
            }

            public struct READER
            {
                readonly DeserializerState ctx;
                public READER(DeserializerState ctx)
                {
                    this.ctx = ctx;
                }

                public static READER create(DeserializerState ctx) => new READER(ctx);
                public static implicit operator DeserializerState(READER reader) => reader.ctx;
                public static implicit operator READER(DeserializerState ctx) => new READER(ctx);
                public uint Number => ctx.ReadDataUInt(0UL, 0U);
                public CapnpGen.Person.PhoneNumber.Type TheType => (CapnpGen.Person.PhoneNumber.Type)ctx.ReadDataUShort(32UL, (ushort)0);
            }

            public class WRITER : SerializerState
            {
                public WRITER()
                {
                    this.SetStruct(1, 0);
                }

                public uint Number
                {
                    get => this.ReadDataUInt(0UL, 0U);
                    set => this.WriteData(0UL, value, 0U);
                }

                public CapnpGen.Person.PhoneNumber.Type TheType
                {
                    get => (CapnpGen.Person.PhoneNumber.Type)this.ReadDataUShort(32UL, (ushort)0);
                    set => this.WriteData(32UL, (ushort)value, (ushort)0);
                }
            }

            [System.CodeDom.Compiler.GeneratedCode("capnpc-csharp", "1.3.0.0"), TypeId(0xd17b0ea013ea75acUL)]
            public enum Type : ushort
            {
                mobile,
                home,
                work
            }
        }
    }
}