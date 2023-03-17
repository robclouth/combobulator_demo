import { useState, useEffect, RefObject } from "react";

const useDrag = <T extends HTMLElement>(ref: RefObject<T>, deps = []) => {
  const [isDragging, setIsDragging] = useState(false);
  const [isHover, setIsHover] = useState(false);

  const [movement, setMovement] = useState({ x: 0, y: 0 });

  const handlePointerDown = (e: PointerEvent) => {
    setIsDragging(true);
  };

  const handlePointerUp = (e: PointerEvent) => {
    setIsDragging(false);
  };

  const handlePointerOver = (e: PointerEvent) => {
    setIsHover(true);
  };

  const handlePointerLeave = (e: PointerEvent) => {
    setIsHover(false);
  };

  const handlePointerMove = (e: PointerEvent) => {
    if (isDragging) {
      setMovement({
        x: e.movementX,
        y: e.movementY,
      });
    }
  };

  useEffect(() => {
    const element = ref.current;
    if (element) {
      element.addEventListener("pointerdown", handlePointerDown);
      document.addEventListener("pointerup", handlePointerUp);
      document.addEventListener("pointermove", handlePointerMove);
      element.addEventListener("pointerover", handlePointerOver);
      element.addEventListener("pointerleave", handlePointerLeave);

      return () => {
        element.removeEventListener("pointerdown", handlePointerDown);
        document.removeEventListener("pointerup", handlePointerUp);
        document.removeEventListener("pointermove", handlePointerMove);
        element.removeEventListener("pointerover", handlePointerOver);
        element.removeEventListener("pointerleave", handlePointerLeave);
      };
    }

    return () => {};
  }, [...deps, isDragging]);

  return { movement, isDragging, isHover };
};

export default useDrag;
