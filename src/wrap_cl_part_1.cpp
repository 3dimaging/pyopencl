#include "wrap_cl.hpp"


using namespace pyopencl;


void pyopencl_expose_part_1(py::module &m)
{
  m.def("get_cl_header_version", get_cl_header_version);

  // {{{ platform
  DEF_SIMPLE_FUNCTION(get_platforms);

  {
    typedef platform cls;
    py::class_<cls>(m, "Platform")
      .DEF_SIMPLE_METHOD(get_info)
      .def("get_devices", &cls::get_devices,
          py::arg("device_type")=CL_DEVICE_TYPE_ALL)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__hash__", &cls::hash)
      PYOPENCL_EXPOSE_TO_FROM_INT_PTR(cl_platform_id)
      ;
  }

  // }}}

  // {{{ device
  {
    typedef device cls;
    py::class_<cls>(m, "Device")
      .DEF_SIMPLE_METHOD(get_info)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__hash__", &cls::hash)
#if defined(cl_ext_device_fission) && defined(PYOPENCL_USE_DEVICE_FISSION)
      .DEF_SIMPLE_METHOD(create_sub_devices_ext)
#endif
#if PYOPENCL_CL_VERSION >= 0x1020
      .DEF_SIMPLE_METHOD(create_sub_devices)
#endif
      PYOPENCL_EXPOSE_TO_FROM_INT_PTR(cl_device_id)
      ;
  }

  // }}}

  // {{{ context

  {
    typedef context cls;
    py::class_<cls, std::shared_ptr<cls>>(m, "Context")
#if 0
      .def(
          py::init(create_context)/*,
          py::arg("devices")=py::object(),
          py::arg("properties")=py::object(),
          py::arg("dev_type")=py::object()
          */
          )
#endif
      .DEF_SIMPLE_METHOD(get_info)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__hash__", &cls::hash)
      PYOPENCL_EXPOSE_TO_FROM_INT_PTR(cl_context)
      ;
  }

  // }}}

  // {{{ command queue
  {
    typedef command_queue cls;
    py::class_<cls>(m, "CommandQueue")
      .def(
        py::init<const context &,
          const device *, cl_command_queue_properties>(),
        py::arg("context"),
        py::arg("device")=py::object(),
        py::arg("properties")=0)
      .DEF_SIMPLE_METHOD(get_info)
#if PYOPENCL_CL_VERSION < 0x1010
      .DEF_SIMPLE_METHOD(set_property)
#endif
      .DEF_SIMPLE_METHOD(flush)
      .DEF_SIMPLE_METHOD(finish)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__hash__", &cls::hash)
      PYOPENCL_EXPOSE_TO_FROM_INT_PTR(cl_command_queue)
      ;
  }

  // }}}

  // {{{ events/synchronization
  {
    typedef event cls;
    py::class_<cls>(m, "Event")
      .DEF_SIMPLE_METHOD(get_info)
      .DEF_SIMPLE_METHOD(get_profiling_info)
      .DEF_SIMPLE_METHOD(wait)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__hash__", &cls::hash)
      PYOPENCL_EXPOSE_TO_FROM_INT_PTR(cl_event)
      ;
  }
  {
    typedef nanny_event cls;
    py::class_<cls, event>(m, "NannyEvent")
      .DEF_SIMPLE_METHOD(get_ward)
      ;
  }

  DEF_SIMPLE_FUNCTION(wait_for_events);

#if PYOPENCL_CL_VERSION >= 0x1020
  m.def("_enqueue_marker_with_wait_list", enqueue_marker_with_wait_list,
      py::arg("queue"), py::arg("wait_for")=py::object()
      );
#endif
  m.def("_enqueue_marker", enqueue_marker,
      py::arg("queue")
      );
  m.def("_enqueue_wait_for_events", enqueue_wait_for_events,
      py::arg("queue"), py::arg("wait_for")=py::object());

#if PYOPENCL_CL_VERSION >= 0x1020
  m.def("_enqueue_barrier_with_wait_list", enqueue_barrier_with_wait_list,
      py::arg("queue"), py::arg("wait_for")=py::object()
      );
#endif
  m.def("_enqueue_barrier", enqueue_barrier, py::arg("queue"));

#if PYOPENCL_CL_VERSION >= 0x1010
  {
    typedef user_event cls;
    py::class_<cls, event>(m, "UserEvent")
#if 0
      .def("__init__", make_constructor(
            create_user_event, py::default_call_policies(), py::args("context")))
#endif
      .DEF_SIMPLE_METHOD(set_status)
      ;
  }
#endif

  // }}}

  // {{{ memory_object

  {
    typedef memory_object_holder cls;
    py::class_<cls>(m, "MemoryObjectHolder")
      .DEF_SIMPLE_METHOD(get_info)
      .def("get_host_array", get_mem_obj_host_array,
          py::arg("shape"),
          py::arg("dtype"),
          py::arg("order")="C")
#if 0
      .def(py::self == py::self)
      .def(py::self != py::self)
#endif
      .def("__hash__", &cls::hash)

      .def_property_readonly("int_ptr", to_int_ptr<cls>,
          "Return an integer corresponding to the pointer value "
          "of the underlying :c:type:`cl_mem`. "
          "Use :meth:`from_int_ptr` to turn back into a Python object."
          "\n\n.. versionadded:: 2013.2\n")
      ;
  }
  {
    typedef memory_object cls;
    py::class_<cls, memory_object_holder>(m, "MemoryObject")
      .DEF_SIMPLE_METHOD(release)
      .def_property_readonly("hostbuf", &cls::hostbuf)

      .def_static("from_int_ptr", memory_object_from_int,
        "(static method) Return a new Python object referencing the C-level " \
        ":c:type:`cl_mem` object at the location pointed to " \
        "by *int_ptr_value*. The relevant :c:func:`clRetain*` function " \
        "will be called." \
        "\n\n.. versionadded:: 2013.2\n") \
      ;
  }

#if PYOPENCL_CL_VERSION >= 0x1020
  m.def("enqueue_migrate_mem_objects", enqueue_migrate_mem_objects,
      py::arg("queue"),
      py::arg("mem_objects"),
      py::arg("flags")=0,
      py::arg("wait_for")=py::object()
      );
#endif

#ifdef cl_ext_migrate_memobject
  m.def("enqueue_migrate_mem_object_ext", enqueue_migrate_mem_object_ext,
      py::arg("queue"),
      py::arg("mem_objects"),
      py::arg("flags")=0,
      py::arg("wait_for")=py::object()
      );
#endif
  // }}}

  // {{{ buffer
  {
    typedef buffer cls;
    py::class_<cls, memory_object>(m, "Buffer")
#if 0
      .def("__init__", make_constructor(create_buffer_py,
            py::default_call_policies(),
            (py::args("context", "flags"),
             py::arg("size")=0,
             py::arg("hostbuf")=py::object()
            )))
#endif
#if PYOPENCL_CL_VERSION >= 0x1010
      .def("get_sub_region", &cls::get_sub_region,
          py::arg("origin"),
          py::arg("size"),
          py::arg("flags")=0
          )
      .def("__getitem__", &cls::getitem)
#endif
      ;
  }

  // }}}

  // {{{ transfers

  // {{{ byte-for-byte
  m.def("_enqueue_read_buffer", enqueue_read_buffer,
      py::arg("queue"),
      py::arg("mem"),
      py::arg("hostbuf"),
      py::arg("device_offset")=0,
      py::arg("wait_for")=py::object(),
      py::arg("is_blocking")=true
      );
  m.def("_enqueue_write_buffer", enqueue_write_buffer,
      py::arg("queue"),
      py::arg("mem"),
      py::arg("hostbuf"),
      py::arg("device_offset")=0,
      py::arg("wait_for")=py::object(),
      py::arg("is_blocking")=true
      );
  m.def("_enqueue_copy_buffer", enqueue_copy_buffer,
      py::arg("queue"),
      py::arg("src"),
      py::arg("dst"),
      py::arg("byte_count")=-1,
      py::arg("src_offset")=0,
      py::arg("dst_offset")=0,
      py::arg("wait_for")=py::object()
      );

  // }}}

  // {{{ rectangular

#if PYOPENCL_CL_VERSION >= 0x1010
  m.def("_enqueue_read_buffer_rect", enqueue_read_buffer_rect,
      py::arg("queue"),
      py::arg("mem"),
      py::arg("hostbuf"),
      py::arg("buffer_origin"),
      py::arg("host_origin"),
      py::arg("region"),
      py::arg("buffer_pitches")=py::object(),
      py::arg("host_pitches")=py::object(),
      py::arg("wait_for")=py::object(),
      py::arg("is_blocking")=true
      );
  m.def("_enqueue_write_buffer_rect", enqueue_write_buffer_rect,
      py::arg("queue"),
      py::arg("mem"),
      py::arg("hostbuf"),
      py::arg("buffer_origin"),
      py::arg("host_origin"),
      py::arg("region"),
      py::arg("buffer_pitches")=py::object(),
      py::arg("host_pitches")=py::object(),
      py::arg("wait_for")=py::object(),
      py::arg("is_blocking")=true
      );
  m.def("_enqueue_copy_buffer_rect", enqueue_copy_buffer_rect,
      py::arg("queue"),
      py::arg("src"),
      py::arg("dst"),
      py::arg("src_origin"),
      py::arg("dst_origin"),
      py::arg("region"),
      py::arg("src_pitches")=py::object(),
      py::arg("dst_pitches")=py::object(),
      py::arg("wait_for")=py::object()
      );
#endif

  // }}}

  // }}}

#if PYOPENCL_CL_VERSION >= 0x1020
  m.def("_enqueue_fill_buffer", enqueue_fill_buffer,
      py::arg("queue"), py::arg("mem"), py::arg("pattern"),
      py::arg("offset"), py::arg("size"),
      py::arg("wait_for")=py::object());
#endif
}

// vim: foldmethod=marker
