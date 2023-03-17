import { Box, Group, Popover, Stack } from "@mantine/core";
import { range } from "@mantine/hooks";
import { observer } from "mobx-react";
import { FC, ReactNode, useContext, useState } from "react";
import { ParametersContext } from "../juceIntegration/JuceIntegration";
import { ParameterModel } from "../juceIntegration/models/ParameterModel";
import { ParameterDial } from "./ParameterDial";

interface Props {
  enabled: boolean;
  parameter: ParameterModel<number>;
  children: ReactNode;
}

export const ModulationPopover: FC<Props> = observer(
  ({ enabled, parameter, children }) => {
    const parameters = useContext(ParametersContext)!;

    const [modulationOpen, setModulationOpen] = useState(false);

    let modulatorsEnabled = false;
    for (let i = 0; i < 5; i++) {
      if ((parameters as any)[`lfo${i + 1}Enabled`].value) {
        modulatorsEnabled = true;
        break;
      }
    }

    return (
      <Popover
        opened={modulationOpen}
        onChange={setModulationOpen}
        withArrow
        arrowSize={15}
        shadow="md"
      >
        <Popover.Target>
          <Box
            onContextMenu={(e) => {
              if (!enabled || !modulatorsEnabled) return;
              e.preventDefault();
              setModulationOpen((o) => !o);
            }}
          >
            {children}
          </Box>
        </Popover.Target>

        <Popover.Dropdown>
          <Stack>
            <Group>
              {range(0, 4).map(
                (index) =>
                  (parameters as any)[`lfo${index + 1}Enabled`].value && (
                    <ParameterDial
                      radius={20}
                      min={-1}
                      max={1}
                      parameter={
                        (parameters as any)[
                          `modAmount_LFO${index + 1}_${parameter.id}`
                        ]
                      }
                      label={`LFO ${index + 1}`}
                      modulatable={false}
                      unit="%"
                      labelValueScale={100}
                    />
                  )
              )}
            </Group>
          </Stack>
        </Popover.Dropdown>
      </Popover>
    );
  }
);
