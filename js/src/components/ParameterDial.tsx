import { Box, Flex, Stack, Text } from "@mantine/core";
import { observer } from "mobx-react";
import { FC, useEffect, useRef } from "react";
import { ArcherElement } from "react-archer";
import useDrag from "../hooks/useDrag";
import { ParameterModel } from "../juceIntegration/models/ParameterModel";
import { ModulationPopover } from "./ModulationPopover";

interface Props {
  min?: number;
  max?: number;
  numSteps?: number;
  sensitivity?: number;
  startAngle?: number;
  endAngle?: number;
  label: string;
  unit?: string;
  decimalPlaces?: number;
  showSign?: boolean;
  labelValueScale?: number;
  valueFormatter?: (value: number) => string;
  radius?: number;
  modulatable?: boolean;
  parameter: ParameterModel<number>;
}

export const ParameterDial: FC<Props> = observer((props) => {
  const {
    parameter,
    min = 0,
    max = 1,
    numSteps = 10,
    sensitivity = 0.01,
    startAngle = 30,
    endAngle = 330,
    label,
    radius = 30,
    unit = "",
    decimalPlaces = 0,
    labelValueScale = 1,
    valueFormatter,
    showSign = false,
    modulatable = true,
  } = props;

  const ref = useRef<HTMLDivElement | null>(null);

  const { movement, isHover, isDragging } = useDrag(ref);

  useEffect(() => {
    let newValue = parameter.value - movement.y * sensitivity * (max - min);
    newValue = Math.min(Math.max(newValue, min), max);
    // newValue =
    //   (Math.round(((newValue - min) / (max - min)) * numSteps) / numSteps) *
    //     (max - min) +
    //   min;
    parameter.value = newValue;
  }, [movement.y]);

  const strokeWidth = 4;
  const margin = 10;
  const width = radius * 2 + strokeWidth * 2 + margin;
  const innerRadius = radius * 0.6;
  const dialTopRadius = radius * 0.45;
  const centerX = width / 2;
  const centerY = width / 2;
  const maxAngle = endAngle - startAngle;
  const angle = ((parameter.value - min) / (max - min)) * maxAngle;

  return (
    <ModulationPopover enabled={modulatable} parameter={parameter}>
      <Stack ref={ref} align="center" spacing={0} pos="relative">
        <Flex w={width} h={width}>
          <svg viewBox={`0 0 ${width} ${width}"`}>
            <filter
              id="dialShadow"
              x="-50%"
              y="-50%"
              width="200%"
              height="200%"
            >
              <feDropShadow
                dx="5"
                dy="5"
                stdDeviation="6"
                floodColor="#000000"
                floodOpacity="0.9"
              />
            </filter>

            <circle
              cx={centerX}
              cy={centerY}
              r={radius}
              stroke="#575555"
              fill="transparent"
              stroke-width="2"
            />
            <circle
              cx={centerX}
              cy={centerY}
              r={innerRadius}
              stroke="#575555"
              fill="transparent"
              stroke-width="1"
            />
            <circle
              cx={centerX}
              cy={centerY}
              r={dialTopRadius}
              fill="#ffffff"
              stroke="transparent"
              style={{ filter: "url(#dialShadow)" }}
            />
            <g
              transform={`rotate(${
                90 + angle + startAngle
              }, ${centerX}, ${centerY})`}
            >
              <circle
                cx={centerX + dialTopRadius - radius * 0.15}
                cy={centerY}
                r={radius * 0.1}
                fill="#b2b2b2"
                stroke="transparent"
              />
            </g>
            {/* <g
              style={{ filter: "url(#shadow)" }}
              fill="none"
              strokeWidth={strokeWidth}
              transform={`rotate(${90 + startAngle}, ${centerX}, ${halfWidth})`}
            >
              <path
                stroke="lightgray"
                d={`M${
                  Math.cos((maxAngle / 180) * Math.PI) * innerRadius + centerX
                },${
                  Math.sin((maxAngle / 180) * Math.PI) * innerRadius + centerY
                } A${innerRadius},${innerRadius} 0 ${
                  maxAngle < 180 ? "0" : "1"
                } 0 ${Math.cos(0) * innerRadius + centerX},${
                  Math.sin(0) * innerRadius + centerY
                }`}
              />
              <path
                stroke="black"
                d={`M${
                  Math.cos((angle / 180) * Math.PI) * innerRadius + centerX
                },${
                  Math.sin((angle / 180) * Math.PI) * innerRadius + centerY
                } A${innerRadius},${innerRadius} 0 ${
                  angle < 180 ? "0" : "1"
                } 0 ${Math.cos(0) * innerRadius + centerX},${
                  Math.sin(0) * innerRadius + centerY
                }`}
              />
            </g> */}
          </svg>
        </Flex>
        <Text fz="xs">
          {isDragging || isHover
            ? valueFormatter
              ? valueFormatter(props.parameter.value)
              : (showSign && props.parameter.value > 0 ? "+" : "") +
                (props.parameter.value * labelValueScale).toLocaleString(
                  undefined,
                  {
                    maximumFractionDigits: decimalPlaces,
                    minimumFractionDigits: decimalPlaces,
                  }
                ) +
                unit
            : label}
        </Text>
        <ArcherElement id={parameter.id}>
          <Box
            pos="absolute"
            left={0}
            right={0}
            top={0}
            bottom={0}
            style={{ pointerEvents: "none" }}
          />
        </ArcherElement>
      </Stack>
    </ModulationPopover>
  );
});
