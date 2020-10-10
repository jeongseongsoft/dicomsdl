/*
 * DICOM software development library (SDL)
 * Copyright (c) 2010-2020, Kim, Tae-Sung. All rights reserved.
 * See copyright.txt for details.
 *
 * _dicomsdl.cc
 */

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/buffer_info.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include "dicom.h"
using namespace dicom;


template <typename T>
struct Iterator {
  typename T::iterator it, end;
  bool first_or_done;
};

py::object dataelement_value(DataElement &de) {
  py::object o;
  switch (de.vr()) {
    case VR::SS:
    case VR::US:
    case VR::SL:
    case VR::UL:
    case VR::SV:
    case VR::UV:
    case VR::IS:
      if (de.vm() > 1)
        o = py::list(py::cast(de.toLongLongVector()));
      else
        o = py::cast(de.toLongLong());
      break;
    case VR::FL:
    case VR::FD:
    case VR::DS:
      if (de.vm() > 1)
        o = py::list(py::cast(de.toDoubleVector()));
      else
        o = py::cast(de.toDouble());
      break;
    case VR::AE:
    case VR::AS:
    case VR::CS:
    case VR::DA:
    case VR::DT:
    case VR::TM:
    case VR::UI:
    case VR::UR:
    case VR::LO:
    case VR::LT:
    case VR::PN:
    case VR::SH:
    case VR::ST:
    case VR::UC:
    case VR::UT:
      if (de.vm() > 1)
        o = py::cast(de.toStringVector());
      else
        o = py::cast(de.toString());
      break;
    default:
      o = py::bytes(de.toBytes());
  }
  return o;
}

PYBIND11_MODULE(_dicomsdl, m) {
  m.attr("version") = py::cast(DICOMSDL_VERSION);

  m.def("open_file", &open_file, "Open a DICOM file from a file.", "filename"_a,
        "load_until"_a = 0xffffffff, "keep_on_error"_a = false);
  m.def("open", &open_file, "Open a DICOM file from a file.", "filename"_a,
        "load_until"_a = 0xffffffff, "keep_on_error"_a = false);
  m.def(
      "open_memory",
      [](py::bytes data, bool copy_data = true, tag_t load_until = 0xffffffff,
         bool keep_on_error = false) {
        char *buffer;
        ssize_t length;
        if (PYBIND11_BYTES_AS_STRING_AND_SIZE(data.ptr(), &buffer, &length))
          py::pybind11_fail("Unable to extract bytes contents!");

        return open_memory((uint8_t *)buffer, (size_t)length, copy_data,
                           load_until, keep_on_error);
      },
      "Open a DICOM file from a string.", "data"_a, "copy_data"_a = true,
      "load_until"_a = 0xffffffff, "keep_on_error"_a = false);

  // Types --------------------------------------------------------------------

  py::class_<VR> vr(m, "VR");
  py::enum_<VR::type>(vr, "type")
      .value("NONE", VR::NONE)
      // VR names are generated by 'codegenerator_valueprepresentations.py'.
      // Place $$Generated_VR code in 'src/python/_dicomsdl.cpp'.
      .value("AE", VR::AE)
      .value("AS", VR::AS)
      .value("AT", VR::AT)
      .value("CS", VR::CS)
      .value("DA", VR::DA)
      .value("DS", VR::DS)
      .value("DT", VR::DT)
      .value("FL", VR::FL)
      .value("FD", VR::FD)
      .value("IS", VR::IS)
      .value("LO", VR::LO)
      .value("LT", VR::LT)
      .value("OB", VR::OB)
      .value("OD", VR::OD)
      .value("OF", VR::OF)
      .value("OL", VR::OL)
      .value("OV", VR::OV)
      .value("OW", VR::OW)
      .value("PN", VR::PN)
      .value("SH", VR::SH)
      .value("SL", VR::SL)
      .value("SQ", VR::SQ)
      .value("SS", VR::SS)
      .value("ST", VR::ST)
      .value("SV", VR::SV)
      .value("TM", VR::TM)
      .value("UC", VR::UC)
      .value("UI", VR::UI)
      .value("UL", VR::UL)
      .value("UN", VR::UN)
      .value("UR", VR::UR)
      .value("US", VR::US)
      .value("UT", VR::UT)
      .value("UV", VR::UV)
      // $$End_VR of generated code.
      .value("PIXSEQ", VR::PIXSEQ)
      .value("OFFSET", VR::OFFSET)
      .value("UNKNOWN", VR::UNKNOWN)
      .export_values()
      .def("__str__", [](const VR::type vr) { return VR::repr(vr); });

  py::class_<UID> uid(m, "UID");
  py::enum_<UID::type>(uid, "type")
      // UID const names are generated by 'codegenerator_builddictionary.py'
      // $$Generated_UID code is placed here.
      .value("VERIFICATION_SOP_CLASS", UID::VERIFICATION_SOP_CLASS)
      .value("IMPLICIT_VR_LITTLE_ENDIAN", UID::IMPLICIT_VR_LITTLE_ENDIAN)
      .value("EXPLICIT_VR_LITTLE_ENDIAN", UID::EXPLICIT_VR_LITTLE_ENDIAN)
      .value("DEFLATED_EXPLICIT_VR_LITTLE_ENDIAN", UID::DEFLATED_EXPLICIT_VR_LITTLE_ENDIAN)
      .value("EXPLICIT_VR_BIG_ENDIAN", UID::EXPLICIT_VR_BIG_ENDIAN)
      .value("JPEG_BASELINE_PROCESS1", UID::JPEG_BASELINE_PROCESS1)
      .value("JPEG_EXTENDED_PROCESS2AND4", UID::JPEG_EXTENDED_PROCESS2AND4)
      .value("JPEG_EXTENDED_PROCESS3AND5", UID::JPEG_EXTENDED_PROCESS3AND5)
      .value("JPEG_SPECTRAL_SELECTION_NONHIERARCHICAL_PROCESS6AND8", UID::JPEG_SPECTRAL_SELECTION_NONHIERARCHICAL_PROCESS6AND8)
      .value("JPEG_SPECTRAL_SELECTION_NONHIERARCHICAL_PROCESS7AND9", UID::JPEG_SPECTRAL_SELECTION_NONHIERARCHICAL_PROCESS7AND9)
      .value("JPEG_FULL_PROGRESSION_NONHIERARCHICAL_PROCESS10AND12", UID::JPEG_FULL_PROGRESSION_NONHIERARCHICAL_PROCESS10AND12)
      .value("JPEG_FULL_PROGRESSION_NONHIERARCHICAL_PROCESS11AND13", UID::JPEG_FULL_PROGRESSION_NONHIERARCHICAL_PROCESS11AND13)
      .value("JPEG_LOSSLESS_NONHIERARCHICAL_PROCESS14", UID::JPEG_LOSSLESS_NONHIERARCHICAL_PROCESS14)
      .value("JPEG_LOSSLESS_NONHIERARCHICAL_PROCESS15", UID::JPEG_LOSSLESS_NONHIERARCHICAL_PROCESS15)
      .value("JPEG_EXTENDED_HIERARCHICAL_PROCESS16AND18", UID::JPEG_EXTENDED_HIERARCHICAL_PROCESS16AND18)
      .value("JPEG_EXTENDED_HIERARCHICAL_PROCESS17AND19", UID::JPEG_EXTENDED_HIERARCHICAL_PROCESS17AND19)
      .value("JPEG_SPECTRAL_SELECTION_HIERARCHICAL_PROCESS20AND22", UID::JPEG_SPECTRAL_SELECTION_HIERARCHICAL_PROCESS20AND22)
      .value("JPEG_SPECTRAL_SELECTION_HIERARCHICAL_PROCESS21AND23", UID::JPEG_SPECTRAL_SELECTION_HIERARCHICAL_PROCESS21AND23)
      .value("JPEG_FULL_PROGRESSION_HIERARCHICAL_PROCESS24AND26", UID::JPEG_FULL_PROGRESSION_HIERARCHICAL_PROCESS24AND26)
      .value("JPEG_FULL_PROGRESSION_HIERARCHICAL_PROCESS25AND27", UID::JPEG_FULL_PROGRESSION_HIERARCHICAL_PROCESS25AND27)
      .value("JPEG_LOSSLESS_HIERARCHICAL_PROCESS28", UID::JPEG_LOSSLESS_HIERARCHICAL_PROCESS28)
      .value("JPEG_LOSSLESS_HIERARCHICAL_PROCESS29", UID::JPEG_LOSSLESS_HIERARCHICAL_PROCESS29)
      .value("JPEG_LOSSLESS_NONHIERARCHICAL_FIRSTORDER_PREDICTION_PROCESS14", UID::JPEG_LOSSLESS_NONHIERARCHICAL_FIRSTORDER_PREDICTION_PROCESS14)
      .value("JPEGLS_LOSSLESS_IMAGE_COMPRESSION", UID::JPEGLS_LOSSLESS_IMAGE_COMPRESSION)
      .value("JPEGLS_LOSSY_NEARLOSSLESS_IMAGE_COMPRESSION", UID::JPEGLS_LOSSY_NEARLOSSLESS_IMAGE_COMPRESSION)
      .value("JPEG2000_IMAGE_COMPRESSION_LOSSLESS_ONLY", UID::JPEG2000_IMAGE_COMPRESSION_LOSSLESS_ONLY)
      .value("JPEG2000_IMAGE_COMPRESSION", UID::JPEG2000_IMAGE_COMPRESSION)
      .value("JPEG2000_PART2_MULTICOMPONENT_IMAGE_COMPRESSION_LOSSLESS_ONLY", UID::JPEG2000_PART2_MULTICOMPONENT_IMAGE_COMPRESSION_LOSSLESS_ONLY)
      .value("JPEG2000_PART2_MULTICOMPONENT_IMAGE_COMPRESSION", UID::JPEG2000_PART2_MULTICOMPONENT_IMAGE_COMPRESSION)
      .value("JPIP_REFERENCED", UID::JPIP_REFERENCED)
      .value("JPIP_REFERENCED_DEFLATE", UID::JPIP_REFERENCED_DEFLATE)
      .value("MPEG2_MAIN_PROFILE_MAIN_LEVEL", UID::MPEG2_MAIN_PROFILE_MAIN_LEVEL)
      .value("MPEG2_MAIN_PROFILE_HIGH_LEVEL", UID::MPEG2_MAIN_PROFILE_HIGH_LEVEL)
      .value("MPEG4_AVC_H264_HIGH_PROFILE_LEVEL41", UID::MPEG4_AVC_H264_HIGH_PROFILE_LEVEL41)
      .value("MPEG4_AVC_H264_BDCOMPATIBLE_HIGH_PROFILE_LEVEL41", UID::MPEG4_AVC_H264_BDCOMPATIBLE_HIGH_PROFILE_LEVEL41)
      .value("MPEG4_AVC_H264_HIGH_PROFILE_LEVEL42_FOR_2D_VIDEO", UID::MPEG4_AVC_H264_HIGH_PROFILE_LEVEL42_FOR_2D_VIDEO)
      .value("MPEG4_AVC_H264_HIGH_PROFILE_LEVEL42_FOR_3D_VIDEO", UID::MPEG4_AVC_H264_HIGH_PROFILE_LEVEL42_FOR_3D_VIDEO)
      .value("MPEG4_AVC_H264_STEREO_HIGH_PROFILE_LEVEL42", UID::MPEG4_AVC_H264_STEREO_HIGH_PROFILE_LEVEL42)
      .value("HEVC_H265_MAIN_PROFILE_LEVEL_51", UID::HEVC_H265_MAIN_PROFILE_LEVEL_51)
      .value("HEVC_H265_MAIN_10_PROFILE_LEVEL_51", UID::HEVC_H265_MAIN_10_PROFILE_LEVEL_51)
      .value("RLE_LOSSLESS", UID::RLE_LOSSLESS)
      .value("RFC_2557_MIME_ENCAPSULATION", UID::RFC_2557_MIME_ENCAPSULATION)
      .value("XML_ENCODING", UID::XML_ENCODING)
      .value("SMPTE_ST_211020_UNCOMPRESSED_PROGRESSIVE_ACTIVE_VIDEO", UID::SMPTE_ST_211020_UNCOMPRESSED_PROGRESSIVE_ACTIVE_VIDEO)
      .value("SMPTE_ST_211020_UNCOMPRESSED_INTERLACED_ACTIVE_VIDEO", UID::SMPTE_ST_211020_UNCOMPRESSED_INTERLACED_ACTIVE_VIDEO)
      .value("SMPTE_ST_211030_PCM_DIGITAL_AUDIO", UID::SMPTE_ST_211030_PCM_DIGITAL_AUDIO)
      .value("PAPYRUS_3_IMPLICIT_VR_LITTLE_ENDIAN", UID::PAPYRUS_3_IMPLICIT_VR_LITTLE_ENDIAN)
      // $$End_UID of generated code.
      .value("UNKNOWN", UID::UNKNOWN)
      .export_values()
      .def("__str__", [](const UID::type vr) { return UID::to_uidname(vr); });
  uid.def_static("uidvalue_to_uidname", &UID::uidvalue_to_uidname,
          "Convert UID value to UID name. e.g. '1.2.840.10008.1.2.1' -> "
          "'Explicit VR Little Endian'");
  uid.def_static("to_uidname", &UID::to_uidname,
          "Convert UID to UID name. e.g. UID.EXPLICIT_VR_LITTLE_ENDIAN -> "
          "'Explicit VR Little Endian'");
  uid.def_static("to_uidvalue", &UID::to_uidvalue,
          "Convert UID to UID value. e.g. UID.EXPLICIT_VR_LITTLE_ENDxIAN -> "
          "'1.2.840.10008.1.2.1'");
  uid.def_static("from_uidvalue", &UID::from_uidvalue,
          "Convert UID valud to tsuid. e.g. '1.2.840.10008.1.2.1' -> "
          "UID.EXPLICIT_VR_LITTLE_ENDIAN");

  py::class_<Config>(m, "Config")
    .def_static("get", &Config::get)
    .def_static("set", &Config::set);

  py::class_<TAG>(m, "TAG")
      .def_static(
          "repr", &TAG::repr,
          "Get Tag's representative string. e.g. 0x00080008 -> (0008,0008)")
      .def_static(
          "str", &TAG::repr,
          "Get Tag's representative string. e.g. 0x00080008 -> (0008,0008)")
      .def_static("get_vr", &TAG::get_vr,
                  "Get Tag's VR. e.g. 0x00080008 -> VR.CS")
      .def_static("keyword", &TAG::keyword,
                  "Get Tag's Keyword. e.g. 0x00080008 -> 'ImageType'")
      .def_static("name", &TAG::name,
                  "Get Tag's Name. e.g. 0x00080008 -> 'Image Type'")
      .def_static("from_keyword", &TAG::from_keyword,
                  "Get a Tag from Keyword. e.g. 'ImageType' -> 0x00080008");

  py::class_<CHARSET> charset(m, "CHARSET");
  py::enum_<CHARSET::type>(charset, "type")
      .value("DEFAULT", CHARSET::DEFAULT)
      .value("ISO_IR_6", CHARSET::ISO_IR_6)
      .value("ISO_IR_100", CHARSET::ISO_IR_100)
      .value("ISO_IR_101", CHARSET::ISO_IR_101)
      .value("ISO_IR_109", CHARSET::ISO_IR_109)
      .value("ISO_IR_110", CHARSET::ISO_IR_110)
      .value("ISO_IR_144", CHARSET::ISO_IR_144)
      .value("ISO_IR_127", CHARSET::ISO_IR_127)
      .value("ISO_IR_126", CHARSET::ISO_IR_126)
      .value("ISO_IR_138", CHARSET::ISO_IR_138)
      .value("ISO_IR_148", CHARSET::ISO_IR_148)
      .value("ISO_IR_13", CHARSET::ISO_IR_13)
      .value("ISO_IR_166", CHARSET::ISO_IR_166)
      .value("ISO_2022_IR_6", CHARSET::ISO_2022_IR_6)
      .value("ISO_2022_IR_100", CHARSET::ISO_2022_IR_100)
      .value("ISO_2022_IR_101", CHARSET::ISO_2022_IR_101)
      .value("ISO_2022_IR_109", CHARSET::ISO_2022_IR_109)
      .value("ISO_2022_IR_110", CHARSET::ISO_2022_IR_110)
      .value("ISO_2022_IR_144", CHARSET::ISO_2022_IR_144)
      .value("ISO_2022_IR_127", CHARSET::ISO_2022_IR_127)
      .value("ISO_2022_IR_126", CHARSET::ISO_2022_IR_126)
      .value("ISO_2022_IR_138", CHARSET::ISO_2022_IR_138)
      .value("ISO_2022_IR_148", CHARSET::ISO_2022_IR_148)
      .value("ISO_2022_IR_13", CHARSET::ISO_2022_IR_13)
      .value("ISO_2022_IR_166", CHARSET::ISO_2022_IR_166)
      .value("ISO_2022_IR_87", CHARSET::ISO_2022_IR_87)
      .value("ISO_2022_IR_159", CHARSET::ISO_2022_IR_159)
      .value("ISO_2022_IR_149", CHARSET::ISO_2022_IR_149)
      .value("ISO_2022_IR_58", CHARSET::ISO_2022_IR_58)
      .value("ISO_IR_192", CHARSET::ISO_IR_192)
      .value("GB18030", CHARSET::GB18030)
      .value("GBK", CHARSET::GBK)
      .value("LATIN1", CHARSET::LATIN1)
      .value("LATIN2", CHARSET::LATIN2)
      .value("LATIN3", CHARSET::LATIN3)
      .value("LATIN4", CHARSET::LATIN4)
      .value("CYRILLIC", CHARSET::CYRILLIC)
      .value("ARABIC", CHARSET::ARABIC)
      .value("GREEK", CHARSET::GREEK)
      .value("HEBREW", CHARSET::HEBREW)
      .value("LATIN5", CHARSET::LATIN5)
      .value("KATAKANA", CHARSET::KATAKANA)
      .value("JISX0201", CHARSET::JISX0201)
      .value("THAI", CHARSET::THAI)
      .value("JISX0208", CHARSET::JISX0208)
      .value("KANJI", CHARSET::KANJI)
      .value("JAPANESE", CHARSET::JAPANESE)
      .value("JISX0212", CHARSET::JISX0212)
      .value("KANJISUPP", CHARSET::KANJISUPP)
      .value("KOREAN", CHARSET::KOREAN)
      .value("KSX1001", CHARSET::KSX1001)
      .value("GB2312", CHARSET::GB2312)
      .value("UTF8", CHARSET::UTF8)
      .value("UNKNOWN", CHARSET::UNKNOWN)
      .export_values()
      .def("term",
           [](const CHARSET::type charset) { return CHARSET::term(charset); })
      .def("description", [](const CHARSET::type charset) {
        return CHARSET::description(charset);
      });
  charset.def_static("from_string",
              (charset_t(*)(const std::string &)) & CHARSET::from_string,
              "Get a Specific Character Set from UID value");

  m.def(
      "convert_to_unicode",
      [](py::bytes data, charset_t charset) {
        char *buffer;
        ssize_t length;
        if (PYBIND11_BYTES_AS_STRING_AND_SIZE(data.ptr(), &buffer, &length))
          py::pybind11_fail("Unable to extract bytes contents!");

        return convert_to_unicode((const char *)buffer, (size_t)length,
                                  charset);
      },
      "Convert bytes to unicode string.");
  m.def(
      "convert_from_unicode",
      [](std::wstring ws, charset_t charset) {
        std::string s = convert_from_unicode(ws.c_str(), ws.size(), charset);
        return py::bytes(s);
      },
      "Convert bytes from unicode string.");

  // class PixelSequence -------------------------------------------------------
  // >>>>>>>>>>>>>>. SEQ->PIXSEQ???????????
  // py::class_<DataSetIter>(m, "DataSetIter")
  //     .def("__iter__", [](DataSetIter &s) -> DataSetIter & { return s; })
  //     .def(
  //         "__next__",
  //         [](DataSetIter &s) -> DataSet * {
  //           if (!s.first_or_done)
  //             ++s.it;
  //           else
  //             s.first_or_done = false;
  //           if (s.it == s.end) {
  //             s.first_or_done = true;
  //             throw py::stop_iteration();
  //           }
  //           return s.it->get();
  //         },
  //         py::return_value_policy::reference_internal);

  py::class_<PixelSequence>(m, "PixelSequence")
      .def("__len__", &PixelSequence::numberOfFrames)
      .def("numberOfFrames", &PixelSequence::numberOfFrames)
      .def("frameEncodedData",
           [](PixelSequence &pixseq, size_t index) {
             Buffer<uint8_t> data = pixseq.frameEncodedData(index);
             return py::bytes((const char *)data.data, data.size);
           })
      .def("copyDecodedFrameData", [](PixelSequence &pixseq, size_t index,
                                      py::array outarr) {
        auto buf = outarr.request();
        if (buf.ndim != 2) {
          throw std::runtime_error("output array's dimension should be 2");
        }
        std::string fmt = buf.format;
        int bytesalloc;
        if (fmt == py::format_descriptor<uint8_t>::format())
          bytesalloc = 1;
        else if (fmt == py::format_descriptor<int16_t>::format() ||
                 fmt == py::format_descriptor<uint16_t>::format())
          bytesalloc = 2;
        else {
          char errmsg[128];
          snprintf(errmsg, 128, "cannot copy to array with format '%s'",
                   fmt.c_str());
          throw std::runtime_error(errmsg);
        }

        if (bytesalloc != buf.strides[1]) {
          char errmsg[128];
          snprintf(
              errmsg, 128,
              "output array's strides[1] (%d) should be (%d) for format '%s'",
              int(buf.strides[1]), bytesalloc, fmt.c_str());
          throw std::runtime_error(errmsg);
        }

        uint8_t *data = (uint8_t *)buf.ptr;
        int rows, cols, rowstrides;
        rows = buf.shape[0];
        cols = buf.shape[1];
        rowstrides = buf.strides[0];
        pixseq.copyDecodedFrameData(index, data, rowstrides * cols, rowstrides);
      });

  // class DataElement ---------------------------------------------------------

  py::class_<DataElement>(m, "DataElement")
      .def("length", &DataElement::length)
      .def("vr", &DataElement::vr)
      .def("tag", &DataElement::tag)
      .def("offset", &DataElement::offset)
      .def("toSequence", &DataElement::toSequence,
           py::return_value_policy::reference_internal)
      .def("toPixelSequence", &DataElement::toPixelSequence,
           py::return_value_policy::reference_internal)
      .def("pointer", &DataElement::pointer,
           py::return_value_policy::reference_internal)
      .def("repr", &DataElement::repr, "max_length"_a = 80)
      .def(
          "toBytes",
          [](DataElement &de, const char *default_value) {
            std::string s = de.toBytes(default_value);
            return py::bytes(s);
          },
          "default_value"_a = "")
      .def("toLong", &DataElement::toLong, "default_value"_a = 0)
      .def("toLongLong", &DataElement::toLongLong, "default_value"_a = 0)
      .def("toLongVector", &DataElement::toLongVector)
      .def("toLongLongVector", &DataElement::toLongLongVector)
      .def("toDouble", &DataElement::toDouble, "default_value"_a = 0.0)
      .def("toDoubleVector", &DataElement::toDoubleVector)
      .def("toString", &DataElement::toString, "default_value"_a = L"")
      .def("toStringVector", &DataElement::toStringVector)
      .def("value", &dataelement_value)
      .def("fromLong", &DataElement::fromLong)
      .def("fromLongLong", &DataElement::fromLongLong)
      .def("fromLongVector", &DataElement::fromLongVector)
      .def("fromLongLongVector", &DataElement::fromLongLongVector)
      .def("fromDouble", &DataElement::fromDouble)
      .def("fromDoubleVector", &DataElement::fromDoubleVector)
      .def("fromString", (void (DataElement::*)(const std::wstring &)) &
                             DataElement::fromString)
      .def("fromStringVector", &DataElement::fromStringVector)
      .def("fromBytes", (void (DataElement::*)(const std::string &)) &
                            DataElement::fromBytes);

  // class DataSet -------------------------------------------------------------

  typedef Iterator<std::map<tag_t, std::unique_ptr<DataElement>>>
      DataElementIterator;
  py::class_<DataElementIterator>(m, "DataElementIterator")
      .def("__iter__",
           [](DataElementIterator &s) -> DataElementIterator & { return s; })
      .def(
          "__next__",
          [](DataElementIterator &s) -> DataElement * {
            if (!s.first_or_done)
              ++s.it;
            else
              s.first_or_done = false;
            if (s.it == s.end) {
              s.first_or_done = true;
              throw py::stop_iteration();
            }
            return s.it->second.get();
          },
          py::return_value_policy::reference_internal);

  py::class_<DataSet>(m, "DataSet")
      .def(py::init<>())
      .def("addDataElement", &DataSet::addDataElement,
           py::return_value_policy::reference_internal, "tag"_a,
           "vr"_a = VR::NONE, "length"_a = 0, "offset"_a = 0)
      .def("getDataElement",
           (DataElement * (DataSet::*)(tag_t)) & DataSet::getDataElement,
           py::return_value_policy::reference_internal)
      .def("getDataElement",
           (DataElement * (DataSet::*)(const char *)) & DataSet::getDataElement,
           py::return_value_policy::reference_internal)
      .def("removeDataElement", &DataSet::removeDataElement)
      .def("attachToFile", &DataSet::attachToFile)
      .def("attachToMemory", &DataSet::attachToMemory)
      .def("getSpecificCharset", &DataSet::getSpecificCharset, "index"_a = 0)
      .def("setSpecificCharset", &DataSet::setSpecificCharset)
      .def("dump", &DataSet::dump, "max_length"_a = 120)
      .def(
          "saveToMemory",
          [](DataSet &ds, bool preamble) {
            std::string s = ds.saveToMemory(preamble);
            return py::bytes(s);
          },
          "preamble"_a = true)
      .def(
          "__iter__",
          [](DataSet &ds) {
            return DataElementIterator{ds.begin(), ds.end(), true};
          },
          py::keep_alive<0, 1>())
      .def("__len__", &DataSet::size)
      .def("copyFrameData",
           [](DataSet &ds, size_t index, py::array outarr) {
             // SamplesPerPixel
             int samplesperpixel = ds.getDataElement(0x00280002)->toLong(1);
             // PlanarConfiguration(0: RGBRGBRGB..., 1:RRR..GGG...BBB...)
             int planarconfig = ds.getDataElement(0x00280006)->toLong();             

             auto outbuf = outarr.request();

             // type checking
             std::string fmt = outbuf.format;
             int bytesalloc;
             if (fmt == py::format_descriptor<uint8_t>::format())
               bytesalloc = 1;
             else if (fmt == py::format_descriptor<int16_t>::format() ||
                      fmt == py::format_descriptor<uint16_t>::format())
               bytesalloc = 2;
             else {
               char errmsg[128];
               snprintf(errmsg, 128, "cannot copy to array with format '%s'",
                        fmt.c_str());
               throw std::runtime_error(errmsg);
             }

             // pixel strides should be same to size of pixel type.
             if (bytesalloc != outbuf.strides[outbuf.ndim - 1]) {
               char errmsg[128];
               snprintf(errmsg, 128,
                        "output array's strides[%d] (%d) should be %d for "
                        "format '%s'",
                        int(outbuf.ndim - 1),
                        int(outbuf.strides[outbuf.ndim - 1]), bytesalloc,
                        fmt.c_str());
               throw std::runtime_error(errmsg);
             }

             // check dimensions
             if (samplesperpixel == 1 && outbuf.ndim == 2) {
               // ok
             } else if (samplesperpixel == 3 && outbuf.ndim == 3) {
               // ok
             } else {
               char errmsg[128];
               snprintf(errmsg, 128,
                        "out array's ndim (%d) / pixeldata's samples per "
                        "pixel (%d) is not supported",
                        int(outbuf.ndim), samplesperpixel);
               throw std::runtime_error(errmsg);
             }

             // check shapes
             int rows = ds.getDataElement(0x00280010)->toLong();
             int cols = ds.getDataElement(0x00280011)->toLong();
             int outbuf_cols, outbuf_rows;
             if (samplesperpixel == 1 || planarconfig == 0) {
               outbuf_rows = outbuf.shape[0];
               outbuf_cols = outbuf.shape[1];
             } else if (samplesperpixel == 3 && planarconfig == 1) {
               outbuf_rows = outbuf.shape[1];
               outbuf_cols = outbuf.shape[2];
             }
             if (rows != outbuf_rows || cols != outbuf_cols) {
               char errmsg[128];
               snprintf(errmsg, 128,
                        "out array shape (%d,%d) != pixel data shape (%d,%d)",
                        outbuf_rows, outbuf_cols, rows, cols);
               throw std::runtime_error(errmsg);
             }

             if (samplesperpixel == 1) {  // gray image ---------------------
               ds.copyFrameData(index, (uint8_t *)outbuf.ptr,
                                outbuf_rows * outbuf.strides[0],
                                outbuf.strides[0]);
             } else {
               if (planarconfig == 0) {  // RGBRGBRGB... --------------------
                 ds.copyFrameData(index, (uint8_t *)outbuf.ptr,
                                  outbuf_rows * outbuf.strides[0],
                                  outbuf.strides[0]);
               } else {  // RRR...GGG...BBB... ------------------------------
                 ds.copyFrameData(index, (uint8_t *)outbuf.ptr,
                                  3 * outbuf.strides[0], outbuf.strides[1]);
               }
             }
           })
      .def("getValues",
           [](DataSet &ds, py::list tags) {
             auto values = py::list();
             for (auto tagobj : tags) {
               DataElement *de;
               if (py::isinstance<py::int_>(tagobj)) {
                 tag_t tag = int(py::reinterpret_borrow<py::int_>(tagobj));
                 de = ds.getDataElement(tag);
               } else if (py::isinstance<py::str>(tagobj)) {
                 std::string tagstr =
                     std::string(py::reinterpret_borrow<py::str>(tagobj));
                 de = ds.getDataElement(tagstr.c_str());
               } else {
                 throw std::runtime_error("tag should be int or str.");
               }
               if (de->isValid())
                 values.append(dataelement_value(*de));
               else
                 values.append(py::cast<py::none>(Py_None));
             }
             return values;
           })
      .def("getPixelDataInfo",
           [](DataSet &ds) {
             py::dict info;
             // Rows
             info["Rows"] = ds.getDataElement(0x00280010)->toLong();
             // Cols
             info["Cols"] = ds.getDataElement(0x00280011)->toLong();
             // NumberOfFrames
             info["NumberOfFrames"] = ds.getDataElement(0x00280008)->toLong(1);
             // SamplesPerPixel
             info["SamplesPerPixel"] = ds.getDataElement(0x00280002)->toLong(1);

             // PlanarConfiguration(0: RGBRGBRGB..., 1:RRR..GGG...BBB...)
             py::object oplanarconfig;
             switch (ds.getDataElement(0x00280006)->toLong(-1)) {
               case 0:
                 info["PlanarConfiguration"] = py::cast("RGBRGBRGB");
                 break;
               case 1:
                 info["PlanarConfiguration"] = py::cast("RRRGGGBBB");
                 oplanarconfig = py::cast("RRRGGGBBB");
                 break;
               default:
                 info["PlanarConfiguration"] = py::cast<py::none>(Py_None);
                 break;
             }

             // BitsAllocated
             int bitsalloc = ds.getDataElement(0x00280100)->toLong();
             int bytesalloc = (bitsalloc > 8 ? 2 : 1);
             info["BitsAllocated"] = bitsalloc;
             info["BytesAllocated"] = bytesalloc;
             // BitsStored
             info["BitsStored"] = ds.getDataElement(0x00280101)->toLong();
             // PixelRepresentation
             bool ifsigned = ds.getDataElement(0x00280103)->toLong();
             info["PixelRepresentation"] = ifsigned;

             std::string dtype;
             if (bytesalloc == 1) {
               dtype = py::format_descriptor<uint8_t>::format();
             } else if (bytesalloc == 2) {
               if (ifsigned)
                 dtype = py::format_descriptor<int16_t>::format();
               else
                 dtype = py::format_descriptor<uint16_t>::format();
             } else {
               throw std::runtime_error("cannot determine dtype");
             }
             info["dtype"] = dtype;

             // PhotometricInterpretation
             info["PhotometricInterpretation"] =
                 ds.getDataElement(0x00280004)->toBytes();

             auto getvalues_in_frame = [&ds](tag_t tag, tag_t macro_tag) {
               DataElement *de;
               py::object objnone = py::cast<py::none>(Py_None);

               // check in root dataset
               de = ds.getDataElement(tag);
               if (de->isValid()) {
                 return py::cast(de->toDouble());
               }

               // check in SharedFunctionalGroupsSequence
               char buf[128];
               snprintf(buf, 128, "52009229.0.%08x.0.%08x", macro_tag, tag);
               de = ds.getDataElement(buf);
               if (de->isValid()) {
                 return py::cast(de->toDouble());
               }

               // check in PerFrameFunctionalGroupsSequence
               Sequence *seq = ds.getDataElement(0x52009230)->toSequence();
               if (seq == nullptr) {
                 return objnone;
               }

               auto li = py::list();
               int seqsize = seq->size();
               for (int i = 0; i < seqsize; ++i) {
                 snprintf(buf, 128, "%08x.0.%08x", macro_tag, tag);
                 de = (*seq)[i]->getDataElement(buf);
                 if (de->isValid()) {
                   li.append(py::cast(de->toDouble()));
                 } else {
                   li.append(objnone);
                 }
               }
               return py::object(li);
             };

             // FrameVOILUTSequence - WindowCenter
             info["WindowCenter"] = getvalues_in_frame(0x00281050, 0x00289132);
             // FrameVOILUTSequence - WindowWidth
             info["WindowWidth"] = getvalues_in_frame(0x00281051, 0x00289132);

             // PixelValueTransformationSequence - RescaleIntercept
             info["RescaleIntercept"] =
                 getvalues_in_frame(0x00281052, 0x00289145);
             // PixelValueTransformationSequence - RescaleSlope
             info["RescaleSlope"] = getvalues_in_frame(0x00281053, 0x00289145);

             return info;
           })
      // .def("pixelData", [](DataSet &ds) {
      //   DataElement *de = ds.getDataElement(0x7fe00010);
      //   if (de->vr() == VR::PIXSEQ) {
      //   } else {
      //     unsigned long rows = ds.getDataElement(0x00280010)->toLong();
      //     unsigned long cols = ds.getDataElement(0x00280011)->toLong();
      //     void *ptr = ds.getDataElement(0x7fe00010)->value_ptr();
      //     int bitsalloc = ds.getDataElement(0x00280100)->toLong();
      //     int ncomps =
      //         ds.getDataElement(0x00280002)->toLong();  // SamplesPerPixel
      //     int ifsigned =
      //         ds.getDataElement(0x00280103)->toLong();  //
      //         PixelRepresentation
      //     unsigned long nframes =
      //         ds.getDataElement(0x00280008)->toLong(1);  // NumberOfFrames
      //     unsigned long bytesalloc = (bitsalloc > 8 ? 2 : 1);
      //     unsigned long ndim;
      //     std::string fmt;
      //     if (bytesalloc == 2) {
      //       if (ds.is_little_endian())
      //         fmt = std::string("<");  // little-endian
      //       else
      //         fmt = std::string(">");  // big-endian
      //       if (ifsigned)
      //         fmt += "h";
      //       else
      //         fmt += "H";
      //     } else
      //       fmt = std::string("B");

      //     if (nframes > 1) {
      //       ndim = 3;
      //       if (bytesalloc == 2) {
      //       return py::array_t<uint16_t>(py::buffer_info(
      //           ptr, bytesalloc, fmt, ndim,
      //           std::vector<size_t>{nframes, cols, rows},
      //           std::vector<size_t>{rows * bytesalloc * cols, rows *
      //           bytesalloc,
      //                               bytesalloc}));
      //       } else {

      //       }
      //     } else {
      //       ndim = 2;
      //       return py::array_t<uint16_t>(py::buffer_info(
      //           ptr, bytesalloc, fmt, ndim, std::vector<size_t>{cols, rows},
      //           std::vector<size_t>{rows * bytesalloc, bytesalloc}));
      //     }
      //   }
      // })
      ;

  // class Sequence
  // ------------------------------------------------------------

  typedef Iterator<std::vector<std::unique_ptr<DataSet>>> DataSetIter;

  py::class_<DataSetIter>(m, "DataSetIter")
      .def("__iter__", [](DataSetIter &s) -> DataSetIter & { return s; })
      .def(
          "__next__",
          [](DataSetIter &s) -> DataSet * {
            if (!s.first_or_done)
              ++s.it;
            else
              s.first_or_done = false;
            if (s.it == s.end) {
              s.first_or_done = true;
              throw py::stop_iteration();
            }
            return s.it->get();
          },
          py::return_value_policy::reference_internal);

  py::class_<Sequence>(m, "Sequence")
      .def("addDataSet", &Sequence::addDataSet,
           py::return_value_policy::reference_internal)
      .def("getDataSet", &Sequence::getDataSet,
           py::return_value_policy::reference_internal)
      .def(
          "__iter__",
          [](Sequence &seq) {
            return DataSetIter{seq.begin(), seq.end(), true};
          },
          py::keep_alive<0, 1>())
      .def("__len__", &Sequence::size)
      .def(
          "__getitem__",
          [](Sequence &ds, size_t index) { return ds.getDataSet(index); },
          py::return_value_policy::reference_internal);

  // Exception
  // -----------------------------------------------------------------

  static py::exception<DicomException> ex(m, "DicomException");
  py::register_exception_translator([](std::exception_ptr p) {
    try {
      if (p) std::rethrow_exception(p);
    } catch (const DicomException &e) {
      ex(e.what());
    }
  });

  // Logging functions
  // ---------------------------------------------------------

  py::class_<LogLevel> loglevel(m, "LogLevel");
  py::enum_<LogLevel::type>(loglevel, "type")
      .value("DISABLE", LogLevel::DISABLE)
      .value("DEBUG", LogLevel::DEBUG)
      .value("WARN", LogLevel::WARN)
      .value("ERROR", LogLevel::ERROR)
      .export_values();

  m.def("set_loglevel", &set_loglevel);
  m.def("get_loglevel", &get_loglevel);
  m.def("set_logger_function", &set_logger_function);
  m.def("log_message", [](LogLevel::type loglevel, const char *message) {
    log_message(loglevel, message);
  });
  m.def("set_default_logger_function", &set_default_logger_function);
}
