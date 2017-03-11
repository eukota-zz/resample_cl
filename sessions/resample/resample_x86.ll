; ModuleID = 'D:/Development/resample_cl/sessions/resample/resample.cl'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir-unknown-unknown"

define cc76 void @PolyEvalOcl(float addrspace(1)* nocapture %coeffs, i32 %order, float addrspace(1)* nocapture %input, float addrspace(1)* nocapture %result) nounwind {
  %1 = tail call cc75 i32 @_Z13get_global_idj(i32 0) nounwind readnone
  %2 = getelementptr inbounds float addrspace(1)* %input, i32 %1
  %3 = load float addrspace(1)* %2, align 4, !tbaa !16
  br label %4

; <label>:4                                       ; preds = %0, %4
  %i.010 = phi i32 [ 0, %0 ], [ %11, %4 ]
  %out.09 = phi float [ 0.000000e+00, %0 ], [ %10, %4 ]
  %5 = getelementptr inbounds float addrspace(1)* %coeffs, i32 %i.010
  %6 = load float addrspace(1)* %5, align 4, !tbaa !16
  %7 = uitofp i32 %i.010 to float
  %8 = tail call cc75 float @_Z3powff(float %3, float %7) nounwind readnone
  %9 = fmul float %6, %8
  %10 = fadd float %out.09, %9
  %11 = add i32 %i.010, 1
  %12 = icmp ugt i32 %11, %order
  br i1 %12, label %13, label %4

; <label>:13                                      ; preds = %4
  %14 = getelementptr inbounds float addrspace(1)* %result, i32 %1
  store float %10, float addrspace(1)* %14, align 4, !tbaa !16
  ret void
}

declare cc75 i32 @_Z13get_global_idj(i32) nounwind readnone

declare cc75 float @_Z3powff(float, float) nounwind readnone

define cc76 void @MatrixMultiplier(float addrspace(1)* nocapture %matrixA, float addrspace(1)* nocapture %matrixB, float addrspace(1)* nocapture %matrixC, i32 %widthA, i32 %widthB) nounwind {
  %1 = tail call cc75 i32 @_Z13get_global_idj(i32 0) nounwind readnone
  %2 = tail call cc75 i32 @_Z13get_global_idj(i32 1) nounwind readnone
  %3 = icmp eq i32 %widthA, 0
  br i1 %3, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %0
  %4 = mul i32 %1, %widthA
  br label %5

; <label>:5                                       ; preds = %.lr.ph, %5
  %inner.014 = phi i32 [ 0, %.lr.ph ], [ %15, %5 ]
  %result.013 = phi float [ 0.000000e+00, %.lr.ph ], [ %14, %5 ]
  %6 = add i32 %inner.014, %4
  %7 = getelementptr inbounds float addrspace(1)* %matrixA, i32 %6
  %8 = load float addrspace(1)* %7, align 4, !tbaa !16
  %9 = mul i32 %inner.014, %widthB
  %10 = add i32 %9, %2
  %11 = getelementptr inbounds float addrspace(1)* %matrixB, i32 %10
  %12 = load float addrspace(1)* %11, align 4, !tbaa !16
  %13 = fmul float %8, %12
  %14 = fadd float %result.013, %13
  %15 = add i32 %inner.014, 1
  %16 = icmp ult i32 %15, %widthA
  br i1 %16, label %5, label %._crit_edge

._crit_edge:                                      ; preds = %5, %0
  %result.0.lcssa = phi float [ 0.000000e+00, %0 ], [ %14, %5 ]
  %17 = mul i32 %1, %widthB
  %18 = add i32 %17, %2
  %19 = getelementptr inbounds float addrspace(1)* %matrixC, i32 %18
  store float %result.0.lcssa, float addrspace(1)* %19, align 4, !tbaa !16
  ret void
}

!opencl.kernels = !{!0, !7}
!opencl.enable.FP_CONTRACT = !{}
!opencl.spir.version = !{!14}
!opencl.ocl.version = !{!14}
!opencl.used.extensions = !{!15}
!opencl.used.optional.core.features = !{!15}
!opencl.compiler.options = !{!15}

!0 = metadata !{void (float addrspace(1)*, i32, float addrspace(1)*, float addrspace(1)*)* @PolyEvalOcl, metadata !1, metadata !2, metadata !3, metadata !4, metadata !5, metadata !6}
!1 = metadata !{metadata !"kernel_arg_addr_space", i32 1, i32 0, i32 1, i32 1}
!2 = metadata !{metadata !"kernel_arg_access_qual", metadata !"none", metadata !"none", metadata !"none", metadata !"none"}
!3 = metadata !{metadata !"kernel_arg_type", metadata !"float*", metadata !"uint", metadata !"float*", metadata !"float*"}
!4 = metadata !{metadata !"kernel_arg_type_qual", metadata !"", metadata !"", metadata !"", metadata !""}
!5 = metadata !{metadata !"kernel_arg_base_type", metadata !"float*", metadata !"uint", metadata !"float*", metadata !"float*"}
!6 = metadata !{metadata !"kernel_arg_name", metadata !"coeffs", metadata !"order", metadata !"input", metadata !"result"}
!7 = metadata !{void (float addrspace(1)*, float addrspace(1)*, float addrspace(1)*, i32, i32)* @MatrixMultiplier, metadata !8, metadata !9, metadata !10, metadata !11, metadata !12, metadata !13}
!8 = metadata !{metadata !"kernel_arg_addr_space", i32 1, i32 1, i32 1, i32 0, i32 0}
!9 = metadata !{metadata !"kernel_arg_access_qual", metadata !"none", metadata !"none", metadata !"none", metadata !"none", metadata !"none"}
!10 = metadata !{metadata !"kernel_arg_type", metadata !"float*", metadata !"float*", metadata !"float*", metadata !"uint", metadata !"uint"}
!11 = metadata !{metadata !"kernel_arg_type_qual", metadata !"", metadata !"", metadata !"", metadata !"", metadata !""}
!12 = metadata !{metadata !"kernel_arg_base_type", metadata !"float*", metadata !"float*", metadata !"float*", metadata !"uint", metadata !"uint"}
!13 = metadata !{metadata !"kernel_arg_name", metadata !"matrixA", metadata !"matrixB", metadata !"matrixC", metadata !"widthA", metadata !"widthB"}
!14 = metadata !{i32 1, i32 2}
!15 = metadata !{}
!16 = metadata !{metadata !"float", metadata !17}
!17 = metadata !{metadata !"omnipotent char", metadata !18}
!18 = metadata !{metadata !"Simple C/C++ TBAA"}
