import { Group, Stack } from "@mantine/core";
import { useContext } from "react";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";
import { Module } from "./Module";
import { ParameterDial } from "../ParameterDial";

export const ThreeBandEQ = () => {
  const parameters = useContext(ParametersContext)!;

  return (
    <Module title="EQ">
      <Stack spacing={0}>
        <ParameterDial
          min={-24}
          max={24}
          parameter={parameters.filterLowGain}
          label="Low"
          decimalPlaces={0}
          unit=" dB"
          showSign
        />
        <ParameterDial
          min={-24}
          max={24}
          parameter={parameters.filterMidGain}
          label="Mid"
          decimalPlaces={0}
          unit=" dB"
          showSign
        />
        <ParameterDial
          min={-24}
          max={24}
          parameter={parameters.filterHighGain}
          label="High"
          decimalPlaces={0}
          unit=" dB"
          showSign
        />
      </Stack>
    </Module>
  );
};
